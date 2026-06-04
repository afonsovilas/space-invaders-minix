#ifndef FONT_H
#define FONT_H

#include <stdint.h>

/**
 * @defgroup font Pixel Font Renderer
 * 5×7 pixel bitmap font for rendering text on screen.
 * Supports ASCII characters from space (0x20) to 'Z' (0x5A).
 * @{
 */

/** Glyph width in pixels at scale 1. */
#define FONT_GLYPH_W 5

/** Glyph height in pixels at scale 1. */
#define FONT_GLYPH_H 7

/** Inter-character gap in pixels at scale 1. */
#define FONT_SPACING 1

/**
 * Draws a single character at the specified position.
 *
 * @param x     Top-left X position
 * @param y     Top-left Y position
 * @param ch    Character to draw (ASCII 0x20–0x5A)
 * @param color Colour index
 * @param scale Pixel multiplier (scale 1 = 5×7 px, scale 2 = 10×14 px, etc.)
 */
void font_draw_char(int x, int y, char ch, uint32_t color, int scale);

/**
 * Draws a null-terminated string at the specified position.
 *
 * @param x     Top-left X position
 * @param y     Top-left Y position
 * @param str   Null-terminated string to draw
 * @param color Colour index
 * @param scale Pixel multiplier
 */
void font_draw_string(int x, int y, const char *str, uint32_t color, int scale);

/**
 * Calculates the pixel width of a string at a given scale.
 * Useful for centering text calculations.
 *
 * @param str   Null-terminated string
 * @param scale Pixel multiplier
 * @return Width in pixels
 */
int font_string_width(const char *str, int scale);

/**
 * Draws a null-terminated string horizontally centred on a column.
 * Equivalent to drawing at: cx - font_string_width(str, scale) / 2
 *
 * @param cx    Centre X column
 * @param y     Top-left Y position
 * @param str   Null-terminated string to draw
 * @param color Colour index
 * @param scale Pixel multiplier
 */
void font_draw_string_centred(int cx, int y, const char *str,
                               uint32_t color, int scale);

/** @} */

#endif /* FONT_H */
