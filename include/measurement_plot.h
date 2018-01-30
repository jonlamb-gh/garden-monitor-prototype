/**
 * @file measurement_plot.h
 * @brief TODO.
 *
 */

#ifndef MEASUREMENT_PLOT_H
#define MEASUREMENT_PLOT_H

#include <VG/openvg.h>

#include "pio_defs.h"
#include "pio_ring.h"
#include "pio.h"

typedef struct
{
    VGfloat x;
    VGfloat y;
    VGfloat w;
    VGfloat h;
    VGfloat dx;
    VGfloat dy;
} mp_grid_s;

typedef struct
{
    VGfloat scale_x;
    VGfloat scale_y;
    VGfloat coord_size;
    unsigned int rgb[3];
} mp_viewport_s;

typedef struct
{
    VGfloat tx;
    VGfloat ty;
    void *font;
    VGfloat font_height;
    VGfloat sensor_offsets[PIO_SENSOR_KIND_COUNT];
    unsigned int digit_rgb[3];
    char string[256];
} mp_legend_s;

typedef struct
{
    mp_grid_s grid;
    mp_legend_s legend;
    mp_viewport_s viewports[PIO_SENSOR_KIND_COUNT];
} measurement_plot_s;

void measurement_plot_apply_default_config(
        measurement_plot_s * const plot);

void measurement_plot_render_pio_ring(
        const pio_s * const pio,
        const pio_ring_s * const ring,
        measurement_plot_s * const plot);

#endif  /* MEASUREMENT_PLOT_H */
