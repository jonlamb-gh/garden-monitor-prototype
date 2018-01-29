/**
 * @file gui.c
 * @brief TODO.
 *
 * TODO - error handling
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

#include "gui_defs.h"
#include "gui.h"

static void render_all(
        gui_s * const gui)
{
    Background(
            gui->background_color_rgb[0],
            gui->background_color_rgb[1],
            gui->background_color_rgb[2]);

    // TODO

    End();
}

int gui_init(
        const unsigned long x,
        const unsigned long y,
        const unsigned long width,
        const unsigned long height,
        gui_s * const gui)
{
    int screen_width = 0;
    int screen_height = 0;

    gui->window.x = x;
    gui->window.y = y;
    gui->window.width = width;
    gui->window.height = height;

    gui->background_color_rgb[0] = 0xFF;
    gui->background_color_rgb[1] = 0xFF;
    gui->background_color_rgb[2] = 0xFF;

    initWindowSize(
            (int) x,
            (int) y,
            (unsigned int) width,
            (unsigned int) height);

    init(&screen_width, &screen_height);

    Start((int) width, (int) height);

    return 0;
}

void gui_fini(
        gui_s * const gui)
{
    (void) gui;

    finish();
}

void gui_render(
        gui_s * const gui)
{
    if(gui != NULL)
    {
        render_all(gui);
    }
}
