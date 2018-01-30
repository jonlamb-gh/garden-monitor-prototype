/**
 * @file gui_defs.h
 * @brief TODO.
 *
 */

#ifndef GUI_DEFS_H
#define GUI_DEFS_H

#include "measurement_plot.h"

typedef struct
{
    unsigned long x;
    unsigned long y;
    unsigned long width;
    unsigned long height;
} window_s;

typedef struct
{
    window_s window;
    unsigned int background_color_rgb[3];
    measurement_plot_s mplot;
} gui_s;

#endif  /* GUI_DEFS_H */
