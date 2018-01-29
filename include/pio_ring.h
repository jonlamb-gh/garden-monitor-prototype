/**
 * @file pio_ring.h
 * @brief TODO.
 *
 */

#ifndef PIO_RING_H
#define PIO_RING_H

#include "pio_defs.h"

typedef struct
{
    unsigned long head;
    unsigned long tail;
    unsigned long mask;
    unsigned long length;
    pio_measurement_s *buffer;
} pio_ring_s;

// must be a power of 2
int pio_ring_alloc(
        const unsigned long length,
        pio_ring_s * const ring);

void pio_ring_free(
        pio_ring_s * const ring);

int pio_ring_init(
        pio_ring_s * const ring);

int pio_ring_flush(
        pio_ring_s * const ring);

int pio_ring_put(
        const pio_measurement_s * const measurement,
        pio_ring_s * const ring);

#endif  /* PIO_RING_H */
