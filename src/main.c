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
#include <popt.h>

#include "pio.h"

enum option_kind
{
    OPTION_VERBOSE = 1,
    OPTION_SERIAL_NUMBER = 2
};

static sig_atomic_t global_exit_signal;

static void sig_handler(int sig)
{
    if(sig == SIGINT)
    {
        global_exit_signal = 1;
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

    // don't start if we've encountered an error
    if(ret != 0)
    {
        global_exit_signal = 1;
    }

    while(global_exit_signal == 0)
    {
        sleep(1);
    }

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
