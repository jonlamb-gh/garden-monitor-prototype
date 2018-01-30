/**
 * @file font.c
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

void *font_get(
        const font_kind font)
{
    Fontinfo *mfont = NULL;

    if(font == FONT_SARIF_TYPE_FACE)
    {
        mfont = &SerifTypeface;
    }
    else
    {
        mfont = &SerifTypeface;
    }

    return (void*) mfont;
}
