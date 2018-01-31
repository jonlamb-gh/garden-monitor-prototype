/**
 * @file pio_ring.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "pio_defs.h"
#include "pio_ring.h"

int pio_ring_alloc(
        const unsigned long length,
        pio_ring_s * const ring)
{
    int ret = 0;

    if((ring == NULL) || (length == 0))
    {
        ret = -1;
    }

    if(ret == 0)
    {
        ring->buffer = calloc((size_t) length, sizeof(*ring->buffer));
        if(ring->buffer == NULL)
        {
            ret = -1;
        }
    }

    if(ret == 0)
    {
        ring->length = length;

        ret = pio_ring_init(ring);
    }

    return ret;
}

void pio_ring_free(
        pio_ring_s * const ring)
{
    if(ring != NULL)
    {
        if(ring->buffer != NULL)
        {
            free(ring->buffer);
            ring->buffer = NULL;
        }

        ring->length = 0;
        ring->mask = 0;
    }
}

int pio_ring_init(
        pio_ring_s * const ring)
{
    int ret = 0;

    if(ring == NULL)
    {
        ret = -1;
    }

    if(ret == 0)
    {
        ring->head = 0;
        ring->tail = 0;
        ring->mask = ring->length -1;
    }

    return ret;
}

int pio_ring_flush(
        pio_ring_s * const ring)
{
    int ret = 0;

    if(ring == NULL)
    {
        ret = -1;
    }

    if(ret == 0)
    {
        ring->head = ring->tail;
    }

    return ret;
}

int pio_ring_put(
        const pio_measurement_s * const measurement,
        pio_ring_s * const ring)
{
    int ret = 0;

    if(ring == NULL)
    {
        ret = -1;
    }

    if(ret == 0)
    {
        const unsigned long new_head =
                (unsigned long) (ring->head + 1) % ring->length;

        ring->head = new_head;

        (void) memcpy(
                &ring->buffer[new_head],
                measurement,
                sizeof(ring->buffer[new_head]));

        if(new_head == ring->tail)
        {
            // overflow
            ring->tail =
                    (unsigned long) (ring->tail + 1) % ring->length;
        }
    }

    return ret;
}
