/**
 * @file gui_buttons.h
 * @brief TODO.
 *
 */

#ifndef GUI_BUTTONS_H
#define GUI_BUTTONS_H

#include <VG/openvg.h>

#include "pio_defs.h"

#define GUI_BUTTONS_LENGTH (PIO_SENSOR_KIND_COUNT * 2)

#define GUI_BUTTON_ID_INVALID (0)
#define GUI_BUTTON_ID_1143_INC (1)
#define GUI_BUTTON_ID_1143_DEC (2)
#define GUI_BUTTON_ID_1127_INC (3)
#define GUI_BUTTON_ID_1127_DEC (4)
#define GUI_BUTTON_ID_1125H_INC (5)
#define GUI_BUTTON_ID_1125H_DEC (6)
#define GUI_BUTTON_ID_1125T_INC (7)
#define GUI_BUTTON_ID_1125T_DEC (8)

typedef struct
{
    unsigned int id;
    unsigned int pressed;
    VGfloat x;
    VGfloat y;
    VGfloat width;
    VGfloat height;
    char label[64];
} gui_button_s;

typedef void (*gui_button_press_callback_fx)(
        const gui_button_s * const button,
        void * const user_data);

typedef struct
{
    unsigned int color_pressed[3];
    unsigned int color_released[3];
    gui_button_s buttons[GUI_BUTTONS_LENGTH];
    gui_button_press_callback_fx press_callback;
    void *press_cb_data;
} gui_buttons_s;

void gui_buttons_apply_default_config(
        gui_buttons_s * const buttons);

void gui_buttons_set_press_callback(
        gui_button_press_callback_fx callback,
        void * const user_data,
        gui_buttons_s * const buttons);

void gui_buttons_handle_press(
        const float x,
        const float y,
        gui_buttons_s * const buttons);

void gui_buttons_handle_release(
        gui_buttons_s * const buttons);

void gui_buttons_render(
        const gui_buttons_s * const buttons);

#endif  /* GUI_BUTTONS_H */
