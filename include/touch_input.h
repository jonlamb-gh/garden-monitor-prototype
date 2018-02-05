/**
 * @file touch_input.h
 * @brief TODO.
 *
 */

#ifndef TOUCH_INPUT_H
#define TOUCH_INPUT_H

#include <pthread.h>
#include <semaphore.h>

#include "events.h"

typedef struct
{
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float width;
    float height;
} ti_surface_s;

typedef struct
{
    pthread_mutex_t mutex;
    unsigned int pressed;
    unsigned long x_raw;
    unsigned long y_raw;
    float x_map;
    float y_map;
} ti_pos_s;

typedef struct
{
    pthread_t thread;
    events_context_s *events_ctx;
    ti_surface_s surface;
    ti_pos_s pos;
    char input_dev[128];
} touch_input_s;

int touch_input_init(
        const char * const input_dev,
        events_context_s * const events_ctx,
        touch_input_s * const touch);

void touch_input_fini(
        touch_input_s * const touch);

int touch_get_last_pos(
        touch_input_s * const touch,
        unsigned long * const tx,
        unsigned long * const ty);

#endif  /* TOUCH_INPUT_H */
