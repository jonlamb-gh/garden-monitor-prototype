/**
 * @file atimer.h
 * @brief TODO.
 *
 */

#ifndef ATIMER_H
#define ATIMER_H

#include <signal.h>
#include <time.h>

typedef void (*atimer_cb)(union sigval data);

typedef struct
{
    timer_t timer_id;
    struct sigevent event;
} atimer_s;

int atimer_create(
        atimer_cb cb,
        void * const cb_data,
        atimer_s * const timer);

int atimer_destroy(
        atimer_s * const timer);

int atimer_set(
        const struct itimerspec * const spec,
        atimer_s * const timer);

void atimer_timespec_set_ms(
        const unsigned long long ms,
        struct timespec * const tspec);

#endif /* ATIMER_H */

