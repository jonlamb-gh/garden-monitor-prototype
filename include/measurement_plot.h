/**
 * @file measurement_plot.h
 * @brief TODO.
 *
 */

#ifndef MEASUREMENT_PLOT_H
#define MEASUREMENT_PLOT_H

#include <time.h>
#include <VG/openvg.h>

#include "pio_defs.h"
#include "pio_ring.h"
#include "pio.h"

typedef struct
{
    struct timespec timestamp;
    time_t time_sec;
    struct tm local_time;
    char date_string[256];
} mp_time_s;

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
    mp_time_s time;
    VGfloat time_y_offset;
    VGfloat sensor_offsets[PIO_SENSOR_KIND_COUNT];
    unsigned int digit_rgb[3];
    char string[256];
} mp_legend_s;

typedef struct
{
    VGfloat offset_x;
    VGfloat offset_tick;
    VGfloat offset_tick_real;
    void *font;
    VGfloat font_height;
    char string[256];
} mp_axis_s;

typedef struct
{
    mp_grid_s grid;
    mp_legend_s legend;
    mp_viewport_s viewports[PIO_SENSOR_KIND_COUNT];
    mp_axis_s axes[PIO_SENSOR_KIND_COUNT];
} measurement_plot_s;

void measurement_plot_apply_default_config(
        measurement_plot_s * const plot);

void measurement_plot_rescale(
        measurement_plot_s * const plot);

void measurement_plot_render_pio_ring(
        const pio_s * const pio,
        const pio_ring_s * const ring,
        measurement_plot_s * const plot);

#endif  /* MEASUREMENT_PLOT_H */
