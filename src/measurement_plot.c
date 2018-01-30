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

#include "measurement_plot.h"

static void draw_coord(
        const VGfloat x,
        const VGfloat y,
        const measurement_plot_s * const plot)
{
    // TESTING
    const VGfloat size = 10.0f;

    StrokeWidth(2.0f);
    Stroke(0xFF, 0xFF, 0xFF, 1.0f);
    Fill(0xFF, 0, 0, 1.0f);
	Circle(x, y, size);
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
    draw_grid(plot);

    draw_coord(50.0f, 50.0f, plot);
}
