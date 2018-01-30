/**
 * @file gui.h
 * @brief TODO.
 *
 */

#ifndef GUI_H
#define GUI_H

#include "gui_defs.h"
#include "pio_ring.h"
#include "pio.h"

int gui_init(
        const unsigned long x,
        const unsigned long y,
        const unsigned long width,
        const unsigned long height,
        gui_s * const gui);

void gui_fini(
        gui_s * const gui);

void gui_render(
        const pio_s * const pio,
        const pio_ring_s * const ring,
        gui_s * const gui);

#endif  /* GUI_H */
