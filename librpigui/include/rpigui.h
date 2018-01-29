/**
 * @file pio.h
 * @brief Phidgets I/O interface.
 *
 */

#ifndef PIO_H
#define PIO_H

#include "gui_defs.h"

int gui_init(
        const unsigned long x,
        const unsigned long y,
        const unsigned long width,
        const unsigned long height,
        gui_s * const gui);

void gui_fini(
        gui_s * const gui);

int gui_render(
        gui_s * const gui);

#endif  /* PIO_H */
