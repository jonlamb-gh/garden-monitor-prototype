/**
 * @file gui_defs.h
 * @brief TODO.
 *
 */

#ifndef GUI_DEFS_H
#define GUI_DEFS_H

#include "gui_buttons.h"
#include "measurement_plot.h"

typedef struct
{
    unsigned long x;
    unsigned long y;
    unsigned long width;
    unsigned long height;
} gui_window_s;

typedef struct
{
    gui_window_s window;
    gui_buttons_s buttons;
    unsigned int background_color_rgb[3];
    measurement_plot_s mplot;
} gui_s;

#endif  /* GUI_DEFS_H */
