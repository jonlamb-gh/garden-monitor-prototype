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
#include <pthread.h>
#include <semaphore.h>
#include <popt.h>

#include "atimer.h"
#include "pio.h"
#include "pio_ring.h"
#include "gui_defs.h"
#include "gui.h"

// TODO - add more options for these
#define DEF_DATA_POLL_INTERVAL_MS (500UL)
#define DEF_RING_BUFFER_LENGTH (128UL)

enum option_e
{
    OPTION_VERBOSE = 1,
    OPTION_SERIAL_NUMBER = 2
};

static volatile sig_atomic_t global_exit_signal;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void sig_handler(int sig)
{
    if(sig == SIGINT)
    {
        global_exit_signal = 1;
    }
}

static void timer_callback(union sigval data)
{
    int status;

    status = pthread_mutex_lock(&mutex);

    if(status == 0)
    {
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

int main(int argc, char **argv)
{
    int ret = 0;
    long serial_number = 0;
    int verbose = 0;
    struct sigaction sigact;
    poptContext opt_ctx;
    pio_s pio;
    pio_ring_s pio_ring;
    atimer_s data_poll_timer;
    gui_s gui;

    const struct itimerspec data_poll_timer_spec =
    {
        .it_value.tv_sec = 0,
        .it_value.tv_nsec = (1000ULL * 1000ULL * DEF_DATA_POLL_INTERVAL_MS),
        .it_interval.tv_sec = 0,
        .it_interval.tv_nsec = (1000ULL * 1000ULL * DEF_DATA_POLL_INTERVAL_MS)
    };

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
            OPTION_SERIAL_NUMBER | POPT_ARGFLAG_OPTIONAL,
            "serial number of device to open",
            "'Phidget serial number (0 means any)'"
        },
        POPT_AUTOHELP
        POPT_TABLEEND
    };

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
            // TODO/TESTING
            (void) verbose;
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
                timer_callback,
                NULL,
                &data_poll_timer);
    }
    
    (void) memset(&gui, 0, sizeof(gui));

    if(ret == 0)
    {
        ret = gui_init(
                0,
                0,
                800,
                480,
                &gui);
    }

    // enable timers
    if(ret == 0)
    {
        ret = atimer_set(
                &data_poll_timer_spec,
                &data_poll_timer);
    }

    // don't start if we've encountered an error
    if(ret != 0)
    {
        global_exit_signal = 1;
    }

    while((global_exit_signal == 0) && (ret == 0))
    {
        pio_measurement_s measurement;

        ret = pthread_mutex_lock(&mutex);

        if(ret == 0)
        {
            ret = pthread_cond_wait(&cond, &mutex);
            ret |= pthread_mutex_unlock(&mutex);
        }

        if(ret == 0)
        {
            // TESTING
            printf(".");
            (void) fflush(stdout);
            ret = pio_poll(&pio, &measurement);
        }

        if(ret == 0)
        {
            ret = pio_ring_put(&measurement, &pio_ring);
        }

        // TODO - render timer
        gui_render(&pio_ring, &gui);
    }

    gui_fini(&gui);

    pio_fini(&pio);

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
