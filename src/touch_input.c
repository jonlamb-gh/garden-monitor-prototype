/**
 * @file touch_input.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/input.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#include "default_config.h"
#include "gui_util.h"
#include "events.h"
#include "touch_input.h"

static inline float map_f(
        const float x,
        const float in_min,
        const float in_max,
        const float out_min,
        const float out_max)
{
    return ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

static inline void map_surface_pos(
        const ti_surface_s * const surf,
        ti_pos_s * const pos)
{
    const int status = pthread_mutex_lock(&pos->mutex);

    if(status == 0)
    {
        pos->x_map = map_f(
                CONSTRAIN((float) pos->x_raw, surf->x_min, surf->x_max),
                surf->x_min,
                surf->x_max,
                0.0f,
                surf->width);

        pos->y_map = map_f(
                CONSTRAIN((float) pos->y_raw, surf->y_min, surf->y_max),
                surf->y_min,
                surf->y_max,
                0.0f,
                surf->height);

        (void) pthread_mutex_unlock(&pos->mutex);
    }
}

static int check_dev_path(
        const char * const path)
{
    return access(path, F_OK | R_OK);
}

static void *event_listener_thread(
        void * const user_data)
{
    int ret;
    int input_fd = -1;
    touch_input_s * const touch = (touch_input_s*) user_data;

    if(touch == NULL)
    {
        ret = -1;
    }

    if(ret == 0)
    {
        input_fd = open(touch->input_dev, O_RDONLY | O_CLOEXEC);
        if(input_fd < 0)
        {
            ret = -1;
            (void) fprintf(
                    stderr,
                    "failed to open input device '%s'\n",
                    touch->input_dev);
        }
    }

    while(ret == 0)
    {
        struct input_event event_data;

        const int status = (int) read(
                input_fd,
                &event_data,
                sizeof(event_data));

        if(status > 0)
        {
            if(event_data.type == EV_ABS)
            {
                if(event_data.code == ABS_X)
                {
                    touch->pos.x_raw = (unsigned long) event_data.value;
                }
                else if(event_data.code == ABS_Y)
                {
                    touch->pos.y_raw = (unsigned long) event_data.value;
                }
            }
            else if(event_data.type == EV_KEY)
            {
                if(event_data.code == BTN_TOUCH)
                {
                    if(event_data.value == 1)
                    {
                        touch->pos.pressed = 1;

                        map_surface_pos(&touch->surface, &touch->pos);

                        events_broadcast(
                                EVENTS_BTN_PRESS,
                                touch->events_ctx);
                    }
                    else
                    {
                        touch->pos.pressed = 0;

                        events_broadcast(
                                EVENTS_BTN_RELEASE,
                                touch->events_ctx);
                    }
                }
            }
        }
    }

    if(input_fd >= 0)
    {
        (void) close(input_fd);
    }

    return NULL;
}

int touch_input_init(
        const char * const input_dev,
        events_context_s * const events_ctx,
        touch_input_s * const touch)
{
    int ret = 0;

    if((input_dev == NULL) || (events_ctx == NULL) || (touch == NULL))
    {
        ret = -1;
    }

    if(ret == 0)
    {
        ret = pthread_mutex_init(&touch->pos.mutex, NULL);
    }

    if(ret == 0)
    {
        touch->surface.x_min = DEF_TOUCH_SURFACE_X_MIN;
        touch->surface.x_max = DEF_TOUCH_SURFACE_X_MAX;
        touch->surface.y_min = DEF_TOUCH_SURFACE_Y_MIN;
        touch->surface.y_max = DEF_TOUCH_SURFACE_Y_MAX;
        touch->surface.width = (float) DEF_SCREEN_WIDTH;
        touch->surface.height = (float) DEF_SCREEN_HEIGHT;
    }

    if(ret == 0)
    {
        ret = check_dev_path(input_dev);
        if(ret != 0)
        {
            (void) fprintf(
                    stderr,
                    "failed to open input device '%s'\n",
                    input_dev);
        }
    }

    if(ret == 0)
    {
        touch->events_ctx = events_ctx;

        (void) strncpy(
                touch->input_dev,
                input_dev,
                sizeof(touch->input_dev));

        ret = pthread_create(
                &touch->thread,
                NULL,
                &event_listener_thread,
                (void*) touch);
        if(ret != 0)
        {
            (void) fprintf(
                    stderr,
                    "failed to create input listener thread - %s\n",
                    strerror(errno));
        }
    }

    return ret;
}

void touch_input_fini(
        touch_input_s * const touch)
{
    if(touch != NULL)
    {
        (void) pthread_cancel(touch->thread);

        (void) pthread_mutex_destroy(&touch->pos.mutex);
    }
}

int touch_get_last_pos(
        touch_input_s * const touch,
        unsigned long * const tx,
        unsigned long * const ty)
{
    int ret = 0;

    if((touch == NULL) || (tx == NULL) || (ty == NULL))
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *tx = 0;
        *ty = 0;

        ret = pthread_mutex_lock(&touch->pos.mutex);

        if(ret == 0)
        {
            const float x_map = touch->pos.x_map;
            const float y_map = touch->pos.y_map;

            ret = pthread_mutex_unlock(&touch->pos.mutex);

            *tx = (unsigned long) x_map;
            *ty = (unsigned long) y_map;
        }
    }

    return ret;
}
