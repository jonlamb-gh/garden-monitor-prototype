/**
 * @file pio_filter.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "pio_defs.h"
#include "pio_filter.h"

int pio_filter_alloc(
        const unsigned long length,
        pio_filter_s * const filter)
{
    int ret = 0;

    if((filter == NULL) || (length == 0))
    {
        ret = -1;
    }

    if(ret == 0)
    {
        filter->buffer = calloc((size_t) length, sizeof(*filter->buffer));
        if(filter->buffer == NULL)
        {
            ret = -1;
        }
    }

    if(ret == 0)
    {
        filter->length = length;

        ret = pio_filter_init(filter);
    }

    return ret;
}

void pio_filter_free(
        pio_filter_s * const filter)
{
    if(filter != NULL)
    {
        if(filter->buffer != NULL)
        {
            free(filter->buffer);
            filter->buffer = NULL;
        }

        filter->head = 0;
        filter->length = 0;
    }
}

int pio_filter_init(
        pio_filter_s * const filter)
{
    int ret = 0;

    if(filter == NULL)
    {
        ret = -1;
    }

    if(ret == 0)
    {
        filter->head = 0;

        unsigned long idx;
        for(idx = 0; idx < filter->length; idx += 1)
        {
            (void) memset(
                    &filter->buffer[idx],
                    0,
                    sizeof(filter->buffer[idx]));
        }
    }

    return ret;
}

int pio_filter_update(
        const pio_measurement_s * const m,
        pio_measurement_s * const m_filtered,
        pio_filter_s * const filter)
{
    int ret = 0;

    if(filter == NULL)
    {
        ret = -1;
    }

    if(ret == 0)
    {
        (void) memcpy(
                &m_filtered->timestamp,
                &m->timestamp,
                sizeof(m->timestamp));

        (void) memcpy(
                &filter->buffer[filter->head],
                m,
                sizeof(filter->buffer[filter->head]));

        filter->head = (filter->head + 1) % filter->length;

        (void) memset(
                m_filtered,
                0,
                sizeof(*m_filtered));

        unsigned long s_idx;
        unsigned long idx;
        for(idx = 0; idx < filter->length; idx += 1)
        {
            for(s_idx = 0; s_idx < PIO_SENSOR_KIND_COUNT; idx += 1)
            {
                m_filtered->values[s_idx] += filter->buffer[idx].values[s_idx];
            }
        }

        for(s_idx = 0; s_idx < PIO_SENSOR_KIND_COUNT; idx += 1)
        {
            m_filtered->values[s_idx] = m_filtered->values[s_idx] / (double) filter->length;
        }
    }

    return ret;
}
