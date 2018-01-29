/**
 * @file atimer.c
 * @brief TODO.
 *
 * TODO:
 * - cb_data
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "atimer.h"

int atimer_create(
        atimer_cb cb,
        void * const cb_data,
        atimer_s * const timer)
{
    int ret = 0;

    if(timer == NULL)
    {
        ret = -1;
    }

    if(ret == 0)
    {
        timer->event.sigev_notify = SIGEV_THREAD;
        timer->event.sigev_value.sival_ptr = cb_data;
        timer->event.sigev_notify_function = cb;
        timer->event.sigev_notify_attributes = NULL;

        ret = timer_create(
                CLOCK_MONOTONIC,
                &timer->event,
                &timer->timer_id);
    }

    return ret;
}

int atimer_destroy(
        atimer_s * const timer)
{
    int ret = 0;

    if(timer == NULL)
    {
        ret = -1;
    }

    if(ret == 0)
    {
        ret = timer_delete(timer->timer_id);
    }

    return ret;
}

int atimer_set(
        const struct itimerspec * const spec,
        atimer_s * const timer)
{
    int ret = 0;

    if(timer == NULL)
    {
        ret = -1;
    }

    if(ret == 0)
    {
        ret = timer_settime(
                timer->timer_id,
                0,
                spec,
                NULL);
    }

    return ret;
}

void atimer_timespec_set_ms(
        const unsigned long long ms,
        struct timespec * const tspec)
{
    if(tspec != NULL)
    {
        if(ms == 0)
        {
            tspec->tv_sec = 0;
            tspec->tv_nsec = 0;
        }
        else
        {
            tspec->tv_sec = (ms / 1000ULL);
            tspec->tv_nsec = ((ms % 1000ULL) * 1000000ULL);
        }
    }
}
