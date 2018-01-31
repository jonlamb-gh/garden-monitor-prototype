/**
 * @file measurement_plot.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <VG/openvg.h>
#include <VG/vgu.h>
#include <fontinfo.h>
#include <shapes.h>

#include "default_config.h"
#include "pio_defs.h"
#include "pio_ring.h"
#include "gui_util.h"
#include "font.h"
#include "measurement_plot.h"

static void render_coord(
        const VGfloat x,
        const VGfloat y,
        const VGfloat size)
{
    StrokeWidth(DEF_COORD_STROKE_WIDTH);
    Stroke(0, 0, 0, 1.0f);

	Circle(x, y, size);
}

static void render_measurement(
        const unsigned long x_index,
        const VGfloat m,
        const mp_viewport_s * const vp)
{
    Fill(vp->rgb[0], vp->rgb[1], vp->rgb[2], DEF_COORD_COLOR_ALPHA);

    render_coord(
            ((VGfloat) x_index * vp->scale_x),
            (m * vp->scale_y),
            vp->coord_size);
}

static void render_grid(
        const mp_grid_s * const grid)
{
    VGfloat ix;
    VGfloat iy;

    Stroke(128, 128, 128, DEF_GRID_COLOR_ALPHA);
    StrokeWidth(2.0f);

    for(ix = grid->x; ix <= (grid->x + grid->w); ix += grid->dx)
    {
        Line(ix, grid->y, ix, grid->y + grid->h);
    }

    for(iy = grid->x; iy <= (grid->y + grid->h); iy += grid->dy)
    {
        Line(grid->x, iy, grid->x + grid->w, iy);
    }
}

static void render_sensor_legend(
        const VGfloat last_value,
        const pio_sensor_s * const sensor,
        const VGfloat sensor_offset,
        const mp_viewport_s * const vp,
        mp_legend_s * const legend)
{
    snprintf(
            legend->string,
            sizeof(legend->string),
            "  - %s - %.2f %s",
            sensor->sensor_info,
            last_value,
            sensor->unit_info.symbol);

    Stroke(0, 0, 0, 1.0f);
    StrokeWidth(0.0f);

    Fill(legend->digit_rgb[0], legend->digit_rgb[1], legend->digit_rgb[2], 1.0f);

    Text(
            legend->tx,
            legend->ty - sensor_offset,
            legend->string,
            *((Fontinfo*) legend->font),
            (int) DEF_FONT_POINT_SIZE);

    Fill(vp->rgb[0], vp->rgb[1], vp->rgb[2], DEF_COORD_COLOR_ALPHA);

    render_coord(
            legend->tx + DEF_LEGEND_ICON_OFFSET,
            legend->ty - sensor_offset + DEF_LEGEND_ICON_OFFSET,
            DEF_COORD_SIZE_LEGEND);
}

static void render_sensor_axis(
        const mp_grid_s * const grid,
        const mp_viewport_s * const vp,
        mp_axis_s * const axis)
{
    VGfloat ty;
    unsigned long idx;

    Fill(vp->rgb[0], vp->rgb[1], vp->rgb[2], 1.0f);
    Stroke(vp->rgb[0], vp->rgb[1], vp->rgb[2], 1.0f);
    StrokeWidth(2.0f);

    Line(axis->offset_x, grid->y, axis->offset_x, grid->y + grid->h);

    for(ty = grid->y, idx = 0; ty <= (grid->y + grid->h); ty += axis->offset_tick, idx += 1)
    {
        Stroke(vp->rgb[0], vp->rgb[1], vp->rgb[2], 1.0f);
        StrokeWidth(2.0f);

        Line(
                axis->offset_x - DEF_AXIS_LENGTH_TICK_HALF,
                ty,
                axis->offset_x + DEF_AXIS_LENGTH_TICK_HALF,
                ty);

        Stroke(0, 0, 0, 1.0f);
        StrokeWidth(0.0f);

        snprintf(
                axis->string,
                sizeof(axis->string),
                "%.0f",
                (VGfloat) idx * axis->offset_tick_real);

        TextEnd(
                axis->offset_x - (2 * DEF_AXIS_LENGTH_TICK_HALF),
                ty - (axis->font_height/2.0f),
                axis->string,
                *((Fontinfo*) axis->font),
                (int) DEF_FONT_POINT_SIZE_AXIS_TICK);
    }
}

void measurement_plot_apply_default_config(
        measurement_plot_s * const plot)
{
    unsigned long idx;

    plot->grid.x = DEF_GRID_OFFSET;
    plot->grid.y = DEF_GRID_OFFSET;
    plot->grid.w = (VGfloat) DEF_SCREEN_WIDTH - (2 * DEF_GRID_OFFSET);
    plot->grid.h = (VGfloat) DEF_SCREEN_HEIGHT - (2 * DEF_GRID_OFFSET);
    plot->grid.dx = DEF_GRID_SPACING;
    plot->grid.dy = DEF_GRID_SPACING * (plot->grid.h/plot->grid.w);

    for(idx = 0; idx < PIO_SENSOR_KIND_COUNT; idx += 1)
    {
        plot->viewports[idx].scale_x =
                (VGfloat) DEF_SCREEN_WIDTH / (VGfloat) DEF_RING_BUFFER_LENGTH;
        plot->viewports[idx].scale_y = 1.0f;
        plot->viewports[idx].coord_size = DEF_COORD_SIZE;
        plot->viewports[idx].rgb[0] = 0;
        plot->viewports[idx].rgb[1] = 0;
        plot->viewports[idx].rgb[2] = 0;
    }

    plot->viewports[PIO_SENSOR_1143].scale_y = DEF_PLOT_SCALE_Y_1143;
    plot->viewports[PIO_SENSOR_1143].rgb[0] = 230;
    plot->viewports[PIO_SENSOR_1143].rgb[1] = 25;
    plot->viewports[PIO_SENSOR_1143].rgb[2] = 75;
    plot->viewports[PIO_SENSOR_1127].scale_y = DEF_PLOT_SCALE_Y_1127;
    plot->viewports[PIO_SENSOR_1127].rgb[0] = 60;
    plot->viewports[PIO_SENSOR_1127].rgb[1] = 180;
    plot->viewports[PIO_SENSOR_1127].rgb[2] = 75;
    plot->viewports[PIO_SENSOR_1125_HUMID].scale_y = DEF_PLOT_SCALE_Y_1125_HUMID;
    plot->viewports[PIO_SENSOR_1125_HUMID].rgb[0] = 0;
    plot->viewports[PIO_SENSOR_1125_HUMID].rgb[1] = 130;
    plot->viewports[PIO_SENSOR_1125_HUMID].rgb[2] = 200;
    plot->viewports[PIO_SENSOR_1125_TEMP].scale_y = DEF_PLOT_SCALE_Y_1125_TEMP;
    plot->viewports[PIO_SENSOR_1125_TEMP].rgb[0] = 245;
    plot->viewports[PIO_SENSOR_1125_TEMP].rgb[1] = 130;
    plot->viewports[PIO_SENSOR_1125_TEMP].rgb[2] = 48;

    plot->legend.font = font_get(FONT_SARIF_TYPE_FACE);
    plot->legend.font_height = TextHeight(
            *((Fontinfo*) plot->legend.font),
            (int) DEF_FONT_POINT_SIZE);
    plot->legend.tx =
            (VGfloat) DEF_LEGEND_OFFSET_X;
    plot->legend.ty =
            (VGfloat) DEF_SCREEN_HEIGHT - DEF_LEGEND_OFFSET_Y;
    plot->legend.digit_rgb[0] = 0;
    plot->legend.digit_rgb[1] = 0;
    plot->legend.digit_rgb[2] = 0;

    for(idx = 0; idx < PIO_SENSOR_KIND_COUNT; idx += 1)
    {
        plot->legend.sensor_offsets[idx] =
                ((VGfloat) idx) * (plot->legend.font_height + (plot->legend.font_height / 6.0f));
    }

    for(idx = 0; idx < PIO_SENSOR_KIND_COUNT; idx += 1)
    {
        plot->axes[idx].font = font_get(FONT_SARIF_TYPE_FACE);
        plot->axes[idx].font_height = TextHeight(
                *((Fontinfo*) plot->axes[idx].font),
                (int) DEF_FONT_POINT_SIZE_AXIS_TICK);
        plot->axes[idx].offset_x =
                DEF_AXIS_OFFSET_X + (((VGfloat) idx) * DEF_AXIS_OFFSET_X_DELTA);
    }

    plot->axes[PIO_SENSOR_1143].offset_tick_real = DEF_AXIS_OFFSET_TICK_1143;
    plot->axes[PIO_SENSOR_1143].offset_tick =
            DEF_AXIS_OFFSET_TICK_1143 * plot->viewports[PIO_SENSOR_1143].scale_y;
    plot->axes[PIO_SENSOR_1127].offset_tick_real = DEF_AXIS_OFFSET_TICK_1127;
    plot->axes[PIO_SENSOR_1127].offset_tick =
            DEF_AXIS_OFFSET_TICK_1127 * plot->viewports[PIO_SENSOR_1127].scale_y;
    plot->axes[PIO_SENSOR_1125_HUMID].offset_tick_real = DEF_AXIS_OFFSET_TICK_1125_HUMID;
    plot->axes[PIO_SENSOR_1125_HUMID].offset_tick =
            DEF_AXIS_OFFSET_TICK_1125_HUMID * plot->viewports[PIO_SENSOR_1125_HUMID].scale_y;
    plot->axes[PIO_SENSOR_1125_TEMP].offset_tick_real = DEF_AXIS_OFFSET_TICK_1125_TEMP;
    plot->axes[PIO_SENSOR_1125_TEMP].offset_tick =
            DEF_AXIS_OFFSET_TICK_1125_TEMP * plot->viewports[PIO_SENSOR_1125_TEMP].scale_y;
}

void measurement_plot_render_pio_ring(
        const pio_s * const pio,
        const pio_ring_s * const ring,
        measurement_plot_s * const plot)
{
    const pio_measurement_s *last_m = NULL;
    unsigned long x_index = 0;
    unsigned long s_idx;
    unsigned long r_idx;

    render_grid(&plot->grid);

    for(r_idx = ring->tail; r_idx != ring->head; r_idx = (r_idx + 1) % ring->length)
    {
        const pio_measurement_s * const m = &ring->buffer[r_idx];
        last_m = m;

        for(s_idx = 0; s_idx < PIO_SENSOR_KIND_COUNT; s_idx += 1)
        {
            render_measurement(
                    x_index,
                    (VGfloat) m->values[s_idx],
                    &plot->viewports[s_idx]);
        }

        x_index += 1;
    }

    for(s_idx = 0; s_idx < PIO_SENSOR_KIND_COUNT; s_idx += 1)
    {
        render_sensor_legend(
                (last_m == NULL) ? 0.0f : (VGfloat) last_m->values[s_idx],
                &pio->sensors[s_idx],
                plot->legend.sensor_offsets[s_idx],
                &plot->viewports[s_idx],
                &plot->legend);

        render_sensor_axis(
                &plot->grid,
                &plot->viewports[s_idx],
                &plot->axes[s_idx]);
    }
}
