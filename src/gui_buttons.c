/**
 * @file gui_buttons.c
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

#include "default_config.h"
#include "gui_util.h"
#include "font.h"
#include "gui_buttons.h"

static int is_contained(
        const float x,
        const float y,
        const gui_button_s * const btn)
{
    int is_cntd = 0;

    if((x >= btn->x) && (x <= (btn->x + btn->width)))
    {
        if((y >= btn->y) && (y <= (btn->y + btn->height)))
        {
            is_cntd = 1;
        }
    }

    return is_cntd;
}

static void render_button(
        const unsigned int * const stroke_color,
        const gui_button_s * const btn)
{
    Stroke(stroke_color[0], stroke_color[1], stroke_color[2], 1.0f);

    StrokeWidth(2.0f);

    Fill(0, 0, 0, 1.0f);

    RoundrectOutline(
            btn->x,
            btn->y,
            btn->width,
            btn->height,
            5.0f,
            5.0f);
}

void gui_buttons_apply_default_config(
        gui_buttons_s * const buttons)
{
    buttons->color_pressed[0] = 0xFF;
    buttons->color_pressed[1] = 0x00;
    buttons->color_pressed[2] = 0x00;
    buttons->color_released[0] = 0x00;
    buttons->color_released[1] = 0x00;
    buttons->color_released[2] = 0x00;

    const VGfloat y0 = DEF_GUI_BUTTON_OFFSET_Y;
    const VGfloat y1 =
            (VGfloat) DEF_SCREEN_HEIGHT - DEF_GUI_BUTTON_SIZE - DEF_GUI_BUTTON_OFFSET_Y;

    const VGfloat x0 = DEF_AXIS_OFFSET_X - (DEF_GUI_BUTTON_SIZE/2);
    const VGfloat dx = DEF_AXIS_OFFSET_X_DELTA;

    buttons->buttons[0].id = GUI_BUTTON_ID_1143_INC;
    buttons->buttons[0].x = x0;
    buttons->buttons[0].y = y1;
    buttons->buttons[0].width = DEF_GUI_BUTTON_SIZE;
    buttons->buttons[0].height = DEF_GUI_BUTTON_SIZE;
    (void) strncpy(
        buttons->buttons[0].label,
        "+",
        sizeof(buttons->buttons[0].label));

    buttons->buttons[1].id = GUI_BUTTON_ID_1143_DEC;
    buttons->buttons[1].x = x0;
    buttons->buttons[1].y = y0;
    buttons->buttons[1].width = DEF_GUI_BUTTON_SIZE;
    buttons->buttons[1].height = DEF_GUI_BUTTON_SIZE;
    (void) strncpy(
        buttons->buttons[1].label,
        "-",
        sizeof(buttons->buttons[1].label));

    buttons->buttons[2].id = GUI_BUTTON_ID_1127_INC;
    buttons->buttons[2].x = x0 + (1 * dx);
    buttons->buttons[2].y = y1;
    buttons->buttons[2].width = DEF_GUI_BUTTON_SIZE;
    buttons->buttons[2].height = DEF_GUI_BUTTON_SIZE;
    (void) strncpy(
        buttons->buttons[2].label,
        "+",
        sizeof(buttons->buttons[2].label));

    buttons->buttons[3].id = GUI_BUTTON_ID_1127_DEC;
    buttons->buttons[3].x = x0 + (1 * dx);
    buttons->buttons[3].y = y0;
    buttons->buttons[3].width = DEF_GUI_BUTTON_SIZE;
    buttons->buttons[3].height = DEF_GUI_BUTTON_SIZE;
    (void) strncpy(
        buttons->buttons[3].label,
        "-",
        sizeof(buttons->buttons[3].label));

    buttons->buttons[4].id = GUI_BUTTON_ID_1125H_INC;
    buttons->buttons[4].x = x0 + (2 * dx);
    buttons->buttons[4].y = y1;
    buttons->buttons[4].width = DEF_GUI_BUTTON_SIZE;
    buttons->buttons[4].height = DEF_GUI_BUTTON_SIZE;
    (void) strncpy(
        buttons->buttons[4].label,
        "+",
        sizeof(buttons->buttons[4].label));

    buttons->buttons[5].id = GUI_BUTTON_ID_1125H_DEC;
    buttons->buttons[5].x = x0 + (2 * dx);
    buttons->buttons[5].y = y0;
    buttons->buttons[5].width = DEF_GUI_BUTTON_SIZE;
    buttons->buttons[5].height = DEF_GUI_BUTTON_SIZE;
    (void) strncpy(
        buttons->buttons[5].label,
        "-",
        sizeof(buttons->buttons[5].label));

    buttons->buttons[6].id = GUI_BUTTON_ID_1125T_INC;
    buttons->buttons[6].x = x0 + (3 * dx);
    buttons->buttons[6].y = y1;
    buttons->buttons[6].width = DEF_GUI_BUTTON_SIZE;
    buttons->buttons[6].height = DEF_GUI_BUTTON_SIZE;
    (void) strncpy(
        buttons->buttons[6].label,
        "+",
        sizeof(buttons->buttons[6].label));

    buttons->buttons[7].id = GUI_BUTTON_ID_1125T_DEC;
    buttons->buttons[7].x = x0 + (3 * dx);
    buttons->buttons[7].y = y0;
    buttons->buttons[7].width = DEF_GUI_BUTTON_SIZE;
    buttons->buttons[7].height = DEF_GUI_BUTTON_SIZE;
    (void) strncpy(
        buttons->buttons[7].label,
        "-",
        sizeof(buttons->buttons[7].label));
}

void gui_buttons_set_press_callback(
        gui_button_press_callback_fx callback,
        void * const user_data,
        gui_buttons_s * const buttons)
{
    buttons->press_callback = callback;
    buttons->press_cb_data = user_data;
}

void gui_buttons_handle_press(
        const float x,
        const float y,
        gui_buttons_s * const buttons)
{
    int is_cntd = 0;

    unsigned long idx;
    for(idx = 0; (idx < GUI_BUTTONS_LENGTH) && (is_cntd == 0); idx += 1)
    {
        is_cntd = is_contained(
                x,
                y,
                &buttons->buttons[idx]);

        if(is_cntd != 0)
        {
            buttons->buttons[idx].pressed = 1;

            if(buttons->press_callback != NULL)
            {
                buttons->press_callback(
                        &buttons->buttons[idx],
                        buttons->press_cb_data);
            }
        }
    }
}

void gui_buttons_handle_release(
        gui_buttons_s * const buttons)
{
    unsigned long idx;
    for(idx = 0; idx < GUI_BUTTONS_LENGTH; idx += 1)
    {
        buttons->buttons[idx].pressed = 0;
    }
}

void gui_buttons_render(
        const gui_buttons_s * const buttons)
{
    unsigned long idx;
    for(idx = 0; idx < GUI_BUTTONS_LENGTH; idx += 1)
    {
        const unsigned int *stroke_color;

        if(buttons->buttons[idx].pressed != 0)
        {
            stroke_color = &buttons->color_pressed[0];
        }
        else
        {
            stroke_color = &buttons->color_released[0];
        }

        render_button(stroke_color, &buttons->buttons[idx]);
    }
}
