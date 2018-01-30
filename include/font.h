/**
 * @file font.h
 * @brief TODO.
 *
 */

#ifndef FONT_H
#define FONT_H

typedef enum
{
    FONT_SARIF_TYPE_FACE = 0,
    FONT_KIND_COUNT
} font_kind;

void *font_get(
        const font_kind font);

#endif  /* FONT_H */
