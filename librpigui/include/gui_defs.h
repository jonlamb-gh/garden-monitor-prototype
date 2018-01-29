/**
 * @file gui_defs.h
 * @brief TODO.
 *
 */

#ifndef GUI_DEFS_H
#define GUI_DEFS_H

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
} gui_s;

#endif  /* GUI_DEFS_H */
