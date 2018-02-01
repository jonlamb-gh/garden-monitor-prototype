/**
 * @file pio_filter.h
 * @brief TODO.
 *
 */

#ifndef PIO_FILTER_H
#define PIO_FILTER_H

#include "pio_defs.h"

typedef struct
{
    unsigned long head;
    unsigned long length;
    pio_measurement_s *buffer;
} pio_filter_s;

int pio_filter_alloc(
        const unsigned long length,
        pio_filter_s * const filter);

void pio_filter_free(
        pio_filter_s * const filter);

int pio_filter_init(
        pio_filter_s * const filter);

int pio_filter_update(
        const pio_measurement_s * const m,
        pio_measurement_s * const m_filtered,
        pio_filter_s * const filter);

#endif  /* PIO_FILTER_H */
