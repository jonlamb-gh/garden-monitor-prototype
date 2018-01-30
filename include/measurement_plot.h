/**
 * @file measurement_plot.h
 * @brief TODO.
 *
 */

#ifndef MEASUREMENT_PLOT_H
#define MEASUREMENT_PLOT_H

#include "pio_defs.h"
#include "pio_ring.h"

typedef struct
{
    // TODO
} measurement_plot_s;

void measurement_plot_apply_default_config(
        measurement_plot_s * const plot);

void measurement_plot_render_ring(
        const pio_ring_s * const ring,
        const measurement_plot_s * const plot);

#endif  /* MEASUREMENT_PLOT_H */
