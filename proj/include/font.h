#ifndef FONT_H
#define FONT_H

#include <stdint.h>

/**
 * @defgroup font Pixel Font
 * 5×7 pixel bitmap font rendered via graphics_draw_rectangle().
 * Supports ASCII 0x20 (space) through 0x5A ('Z').
 *
 * Usage:
 *   font_draw_string(x, y, "SCORE 100", 0x0E, 2);  // scale 2 = 10×14 px per char
 * @{
 */

#define FONT_GLYPH_W 5  /**< Glyph width  in pixels at scale 1 */
#define FONT_GLYPH_H 7  /**< Glyph height in pixels at scale 1 */
#define FONT_SPACING 1  /**< Inter-character gap in pixels at scale 1 */

/**
 * Draw a single character.
 * @param x      Top-left X position
 * @param y      Top-left Y position
 * @param ch     Character to draw (ASCII 0x20–0x5A)
 * @param color  Colour index (8-bit VGA palette)
 * @param scale  Pixel multiplier (1=5×7 px, 2=10×14 px, …)
 */
void font_draw_char(int x, int y, char ch, uint32_t color, int scale);

/**
 * Draw a null-terminated string.
 */
void font_draw_string(int x, int y, const char *str, uint32_t color, int scale);

/**
 * Compute the pixel width of a string at a given scale.
 * Useful for centering: x = screen_w/2 - font_string_width(s,scale)/2
 */
int font_string_width(const char *str, int scale);

/**@}*/

#endif /* FONT_H */
