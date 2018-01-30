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

#include "pio_defs.h"
#include "pio_ring.h"
#include "measurement_plot.h"

static void vp_to_screen(
        const VGfloat x,
        const VGfloat y,
        const VGfloat width,
        const VGfloat height,
        VGfloat * const sx,
        VGfloat * const sy)
{
    //*sx = (x + (width/2.0f));
    //*sy = (y + (height/2.0f));

    *sx = x;
    *sy = y;
}

static void draw_coord(
        const VGfloat x,
        const VGfloat y,
        const measurement_plot_s * const plot)
{
    // TESTING
    const VGfloat size = 10.0f;

    VGfloat sx;
    VGfloat sy;

    vp_to_screen(x, y, 800.0f, 480.0f, &sx, &sy);

    StrokeWidth(2.0f);
    Stroke(0xFF, 0xFF, 0xFF, 1.0f);
    Fill(0xFF, 0, 0, 1.0f);
	Circle(sx, sy, size);
}

static void draw_grid(
        const measurement_plot_s * const plot)
{
    VGfloat ix;
    VGfloat iy;

    // TESTING
    const VGfloat x = 0.0f;
    const VGfloat y = 0.0f;
    const VGfloat w = 800.0f;
    const VGfloat h = 480.0f;
    const VGfloat dx = 25.0f;
    const VGfloat dy = dx * (h/w);

    Stroke(128, 128, 128, 0.5f);
    StrokeWidth(2.0f);

    for(ix = x; ix <= (x + w); ix += dx)
    {
        Line(ix, y, ix, y + h);
    }

    for(iy = x; iy <= (y + h); iy += dy)
    {
        Line(x, iy, x + w, iy);
    }
}

void measurement_plot_apply_default_config(
        measurement_plot_s * const plot)
{

}

void measurement_plot_render_ring(
        const pio_ring_s * const ring,
        const measurement_plot_s * const plot)
{
    unsigned long cnt = 0;

    draw_grid(plot);

    unsigned long idx;
    for(idx = ring->tail; idx != ring->head; idx = (idx + 1) & ring->mask)
    {
        const pio_measurement_s * const m = &ring->buffer[idx];

        //printf("v[%lu] = %f\n", idx, m->values[PIO_SENSOR_1125_TEMP]);
        //printf("v[%lu] = %f\n", idx, m->values[PIO_SENSOR_1143]);

        //PIO_SENSOR_1125_TEMP
        
        const VGfloat x = (VGfloat) (cnt * 800.0f/128.0f);
        const VGfloat y = (VGfloat) m->values[PIO_SENSOR_1143] * 100.0f;
        //const VGfloat y = (VGfloat) m->values[PIO_SENSOR_1143];

        draw_coord(x, y, plot);

        cnt += 1;
    }
}
