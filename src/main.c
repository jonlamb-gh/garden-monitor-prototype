/**
 * @file main.c
 * @brief Main.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <popt.h>
#include <zlog.h>

#include "default_config.h"
#include "atimer.h"
#include "pio.h"
#include "pio_ring.h"
#include "gui_defs.h"
#include "gui.h"

#define TIMER_EVENT_DATA_POLL (0x01)
#define TIMER_EVENT_GUI_REDRAW (0x02)

enum option_e
{
    OPTION_VERBOSE = 1,
    OPTION_SERIAL_NUMBER,
    OPTION_CSV_LOG,
    OPTION_SCREEN_SHOT_FILE
};

static volatile sig_atomic_t global_exit_signal;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void sig_handler(
        int sig)
{
    if(sig == SIGINT)
    {
        global_exit_signal = 1;
    }
}

static void timer_broadcast(
        const uint32_t event_flags,
        volatile uint32_t * const events)
{
    int status;

    status = pthread_mutex_lock(&mutex);

    if(status == 0)
    {
        *events |= event_flags;

        status = pthread_cond_signal(&cond);
    }

    if(status == 0)
    {
        status = pthread_mutex_unlock(&mutex);
    }

    if(status != 0)
    {
        exit(1);
    }
}

static void data_poll_timer_callback(
        union sigval data)
{
    if(data.sival_ptr != NULL)
    {
        timer_broadcast(
                TIMER_EVENT_DATA_POLL,
                (volatile uint32_t*) data.sival_ptr);
    }
}

static void gui_redraw_timer_callback(
        union sigval data)
{
    if(data.sival_ptr != NULL)
    {
        timer_broadcast(
                TIMER_EVENT_GUI_REDRAW,
                (volatile uint32_t*) data.sival_ptr);
    }
}

static void drop_root_privileges(void)
{
    if(getuid() == 0)
    {
        printf("dropping root privileges\n");

        if(setgid(getgid()) != 0)
        {
            (void) fprintf(stderr, "failed to drop privileges: %s\n", strerror(errno));
            exit(-1);
        }

        if(setuid(getuid()) != 0)
        {
            (void) fprintf(stderr, "failed to drop privileges: %s\n", strerror(errno));
            exit(-1);
        }
    }
}


int main(
        int argc,
        char **argv)
{
    int ret = 0;
    int verbose = 0;
    long serial_number = 0;
    char *zlog_cat_name = NULL;
    char *screen_shot_file = NULL;
    int zlog_enabled = 0;
    volatile uint32_t timer_signal_events = 0;
    struct sigaction sigact;
    poptContext opt_ctx;
    pio_s pio;
    pio_ring_s pio_ring;
    atimer_s data_poll_timer;
    atimer_s gui_redraw_timer;
    pio_measurement_s measurement;
    gui_s gui;
    struct itimerspec data_poll_timer_spec;
    struct itimerspec gui_redraw_timer_spec;

    const struct poptOption OPTIONS_TABLE[] =
    {
        {
            "verbose",
            'v',
            POPT_ARG_NONE,
            NULL,
            OPTION_VERBOSE,
            "enable verbose output",
            NULL
        },
        {
            "serial-number",
            's',
            POPT_ARG_LONG,
            &serial_number,
            OPTION_SERIAL_NUMBER,
            "serial number of device to open",
            "'Phidget serial number (0 means any)'"
        },
        {
            "log",
            'o',
            POPT_ARG_STRING | POPT_ARGFLAG_OPTIONAL,
            &zlog_cat_name,
            OPTION_CSV_LOG,
            "enable zlog CSV category logging",
            "'category name' default is 'gm'"
        },
        {
            "screen-shot",
            'r',
            POPT_ARG_STRING,
            &screen_shot_file,
            OPTION_SCREEN_SHOT_FILE,
            "on exit, write the current frame buffer to file (raw RGBA 32)",
            "'file path'"
        },
        POPT_AUTOHELP
        POPT_TABLEEND
    };

    drop_root_privileges();

    opt_ctx = poptGetContext(
            NULL,
            argc,
            (const char**) argv,
            OPTIONS_TABLE,
            0);

    int opt_ret;
    while((opt_ret = poptGetNextOpt(opt_ctx)) >= 0)
    {
        if(opt_ret == OPTION_VERBOSE)
        {
            verbose = 1;
        }
        else if(opt_ret == OPTION_SERIAL_NUMBER)
        {
            if(serial_number <= 0)
            {
                (void) fprintf(
                        stderr,
                        "serial number must be greater than zero\n");
                poptPrintUsage(opt_ctx, stderr, 0);
                poptFreeContext(opt_ctx);
                exit(EXIT_FAILURE);
            }
        }
        else if(opt_ret == OPTION_CSV_LOG)
        {
            zlog_enabled = 1;
        }
    }

    if(opt_ret < -1)
    {
        (void) fprintf(
                stderr,
                "argument error '%s': %s\n\n",
                poptBadOption(opt_ctx, POPT_BADOPTION_NOALIAS),
                poptStrerror(opt_ret));
        poptPrintUsage(opt_ctx, stderr, 0);
        poptFreeContext(opt_ctx);
        exit(EXIT_FAILURE);
    }

    poptFreeContext(opt_ctx);

    global_exit_signal = 0;
    (void) memset(&sigact, 0, sizeof(sigact));

    sigact.sa_flags = SA_RESTART;
    sigact.sa_handler = sig_handler;

    if(ret == 0)
    {
        ret = sigaction(SIGINT, &sigact, 0);
    }

    (void) memset(&pio, 0, sizeof(pio));

    if((zlog_enabled != 0) && (ret == 0))
    {
        const char * const category =
                (zlog_cat_name != NULL) ? zlog_cat_name : "gm";

        if(verbose != 0)
        {
            (void) fprintf(
                    stdout,
                    "enabled zlog CSV logging with category '%s'\n",
                    category);
        }

        ret = dzlog_init(DEF_ZLOG_CONF_FILE, category);
        if(ret != 0)
        {
            (void) fprintf(stderr, "failed to open '%s'\n", DEF_ZLOG_CONF_FILE);
        }
    }

    if(ret == 0)
    {
        ret = pio_init(serial_number, &pio);
    }

    (void) memset(&pio_ring, 0, sizeof(pio_ring));

    if(ret == 0)
    {
        ret = pio_ring_alloc(DEF_RING_BUFFER_LENGTH, &pio_ring);
    }

    (void) memset(&data_poll_timer, 0, sizeof(data_poll_timer));

    if(ret == 0)
    {
        ret = atimer_create(
                data_poll_timer_callback,
                (void*) &timer_signal_events,
                &data_poll_timer);
    }

    if(ret == 0)
    {
        ret = atimer_create(
                gui_redraw_timer_callback,
                (void*) &timer_signal_events,
                &gui_redraw_timer);
    }

    if(ret == 0)
    {
        atimer_timespec_set_ms(
                DEF_TIMER_START_DELAY_MS,
                &data_poll_timer_spec.it_value);
    }

    if(ret == 0)
    {
        atimer_timespec_set_ms(
                DEF_DATA_POLL_INTERVAL_MS,
                &data_poll_timer_spec.it_interval);
    }

    if(ret == 0)
    {
        atimer_timespec_set_ms(
                DEF_TIMER_START_DELAY_MS,
                &gui_redraw_timer_spec.it_value);
    }

    if(ret == 0)
    {
        atimer_timespec_set_ms(
                DEF_GUI_REDRAW_INTERVAL_MS,
                &gui_redraw_timer_spec.it_interval);
    }

    (void) memset(&gui, 0, sizeof(gui));

    if(ret == 0)
    {
        ret = gui_init(
                0,
                0,
                DEF_SCREEN_WIDTH,
                DEF_SCREEN_HEIGHT,
                &gui);
    }

    // enable timers
    if(ret == 0)
    {
        ret = atimer_set(
                &data_poll_timer_spec,
                &data_poll_timer);
    }

    if(ret == 0)
    {
        ret = atimer_set(
                &gui_redraw_timer_spec,
                &gui_redraw_timer);
    }

    // don't start if we've encountered an error
    if(ret != 0)
    {
        global_exit_signal = 1;
    }

    while((global_exit_signal == 0) && (ret == 0))
    {
        int poll_for_data = 0;
        int redraw_gui = 0;

        ret = pthread_mutex_lock(&mutex);

        if(ret == 0)
        {
            ret = pthread_cond_wait(&cond, &mutex);

            if((timer_signal_events & TIMER_EVENT_DATA_POLL) != 0)
            {
                timer_signal_events &= ~TIMER_EVENT_DATA_POLL;
                poll_for_data = 1;
            }

            if((timer_signal_events & TIMER_EVENT_GUI_REDRAW) != 0)
            {
                timer_signal_events &= ~TIMER_EVENT_GUI_REDRAW;
                redraw_gui = 1;
            }

            ret |= pthread_mutex_unlock(&mutex);
        }

        if((poll_for_data != 0) && (ret == 0))
        {
            ret = pio_poll(&pio, &measurement);

            if(ret == 0)
            {
                ret = pio_ring_put(&measurement, &pio_ring);
            }
        }

        if((poll_for_data != 0) && (zlog_enabled != 0) && (ret == 0))
        {
            dzlog_info(
                    "%lu.%lu,%f,%f,%f,%f",
                    (unsigned long) measurement.timestamp.tv_sec,
                    (unsigned long) measurement.timestamp.tv_nsec,
                    measurement.values[0],
                    measurement.values[1],
                    measurement.values[2],
                    measurement.values[3]);
        }

        if(redraw_gui != 0)
        {
            gui_render(&pio, &pio_ring, &gui);
        }
    }

    gui_fini(screen_shot_file, &gui);

    pio_fini(&pio);

    if(zlog_enabled != 0)
    {
        zlog_fini();
    }

    if(zlog_cat_name != NULL)
    {
        free(zlog_cat_name);
    }

    if(screen_shot_file != NULL)
    {
        free(screen_shot_file);
    }

    if(ret == 0)
    {
        ret = EXIT_SUCCESS;
    }
    else
    {
        ret = EXIT_FAILURE;
    }

    return ret;
}
