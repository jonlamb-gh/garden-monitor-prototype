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

#include "default_config.h"
#include "gui_defs.h"
#include "gui_util.h"
#include "gui_buttons.h"
#include "measurement_plot.h"
#include "gui.h"

static VGfloat remap_scale(
        const VGfloat current,
        const VGfloat delta,
        const VGfloat fallback)
{
    float new_val;

    new_val = current + delta;

    if(new_val <= 0.0f)
    {
        new_val = fallback;
    }

    return new_val;
}

static void button_press_callback(
        const gui_button_s * const btn,
        void * const user_data)
{
    gui_s * const gui = (gui_s*) user_data;

    const VGfloat scales[] =
    {
        [PIO_SENSOR_1143] = 0.5f,
        [PIO_SENSOR_1127] = 0.5f,
        [PIO_SENSOR_1125_HUMID] = 0.5f,
        [PIO_SENSOR_1125_TEMP] = 0.5f
    };

    if(gui != NULL)
    {
        if(btn->id == GUI_BUTTON_ID_1143_INC)
        {
            gui->mplot.viewports[PIO_SENSOR_1143].scale_y = remap_scale(
                    gui->mplot.viewports[PIO_SENSOR_1143].scale_y,
                    scales[PIO_SENSOR_1143],
                    DEF_PLOT_SCALE_Y_1143);
        }
        else if(btn->id == GUI_BUTTON_ID_1143_DEC)
        {
            gui->mplot.viewports[PIO_SENSOR_1143].scale_y = remap_scale(
                    gui->mplot.viewports[PIO_SENSOR_1143].scale_y,
                    -scales[PIO_SENSOR_1143],
                    DEF_PLOT_SCALE_Y_1143);
        }
        else if(btn->id == GUI_BUTTON_ID_1127_INC)
        {
            gui->mplot.viewports[PIO_SENSOR_1127].scale_y = remap_scale(
                    gui->mplot.viewports[PIO_SENSOR_1127].scale_y,
                    scales[PIO_SENSOR_1127],
                    DEF_PLOT_SCALE_Y_1127);
        }
        else if(btn->id == GUI_BUTTON_ID_1127_DEC)
        {
            gui->mplot.viewports[PIO_SENSOR_1127].scale_y = remap_scale(
                    gui->mplot.viewports[PIO_SENSOR_1127].scale_y,
                    -scales[PIO_SENSOR_1127],
                    DEF_PLOT_SCALE_Y_1127);
        }
        else if(btn->id == GUI_BUTTON_ID_1125H_INC)
        {
            gui->mplot.viewports[PIO_SENSOR_1125_HUMID].scale_y = remap_scale(
                    gui->mplot.viewports[PIO_SENSOR_1125_HUMID].scale_y,
                    scales[PIO_SENSOR_1125_HUMID],
                    DEF_PLOT_SCALE_Y_1125_HUMID);
        }
        else if(btn->id == GUI_BUTTON_ID_1125H_DEC)
        {
            gui->mplot.viewports[PIO_SENSOR_1125_HUMID].scale_y = remap_scale(
                    gui->mplot.viewports[PIO_SENSOR_1125_HUMID].scale_y,
                    -scales[PIO_SENSOR_1125_HUMID],
                    DEF_PLOT_SCALE_Y_1125_HUMID);
        }
        else if(btn->id == GUI_BUTTON_ID_1125T_INC)
        {
            gui->mplot.viewports[PIO_SENSOR_1125_TEMP].scale_y = remap_scale(
                    gui->mplot.viewports[PIO_SENSOR_1125_TEMP].scale_y,
                    scales[PIO_SENSOR_1125_TEMP],
                    DEF_PLOT_SCALE_Y_1125_TEMP);
        }
        else if(btn->id == GUI_BUTTON_ID_1125T_DEC)
        {
            gui->mplot.viewports[PIO_SENSOR_1125_TEMP].scale_y = remap_scale(
                    gui->mplot.viewports[PIO_SENSOR_1125_TEMP].scale_y,
                    -scales[PIO_SENSOR_1125_TEMP],
                    DEF_PLOT_SCALE_Y_1125_TEMP);
        }

        measurement_plot_rescale(&gui->mplot);
    }
}

static void render_all(
        const pio_s * const pio,
        const pio_ring_s * const ring,
        gui_s * const gui)
{
    Background(
            gui->background_color_rgb[0],
            gui->background_color_rgb[1],
            gui->background_color_rgb[2]);

    measurement_plot_render_pio_ring(
            pio,
            ring,
            &gui->mplot);

    gui_buttons_render(&gui->buttons);

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

    // init after OpenVG init, so we can get the font heights
    measurement_plot_apply_default_config(&gui->mplot);

    gui_buttons_apply_default_config(&gui->buttons);

    gui_buttons_set_press_callback(
            &button_press_callback,
            (void*) gui,
            &gui->buttons);

    Start((int) width, (int) height);

    return 0;
}

void gui_fini(
        const char * const screen_shot_file,
        gui_s * const gui)
{
    (void) gui;

    if(screen_shot_file != NULL)
    {
        SaveEnd(screen_shot_file);
    }

    finish();
}

void gui_render(
        const pio_s * const pio,
        const pio_ring_s * const ring,
        gui_s * const gui)
{
    if(gui != NULL)
    {
        render_all(pio, ring, gui);
    }
}
