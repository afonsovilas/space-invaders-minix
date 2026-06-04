#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <lcom/lcf.h>

/**
 * @defgroup graphics Graphics Subsystem
 * VBE graphics driver with double-buffering (mode 0x105: 1024×768, 8-bit indexed colour).
 * @{
 */

/**
 * Initializes the graphics system and allocates the back-buffer.
 * Must be called once before any drawing operations.
 *
 * @param mode VBE mode (typically 0x105)
 * @return 0 on success, non-zero on failure
 */
int graphics_init(uint16_t mode);

/**
 * Clears the entire back-buffer with a single colour.
 *
 * @param color Colour index
 */
void graphics_clear(uint32_t color);

/**
 * Swaps the back-buffer to VRAM (double-buffer flush).
 * Call this once per frame after all drawing is complete.
 */
void graphics_flush(void);

/**
 * Draws a single pixel in the back-buffer.
 *
 * @param x     X coordinate
 * @param y     Y coordinate
 * @param color Colour index
 * @return 0 on success, non-zero on failure
 */
int graphics_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/**
 * Draws a horizontal line in the back-buffer.
 *
 * @param x     Starting X coordinate
 * @param y     Y coordinate
 * @param len   Line length in pixels
 * @param color Colour index
 * @return 0 on success, non-zero on failure
 */
int graphics_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/**
 * Draws a filled rectangle in the back-buffer.
 *
 * @param x      Top-left X coordinate
 * @param y      Top-left Y coordinate
 * @param width  Rectangle width in pixels
 * @param height Rectangle height in pixels
 * @param color  Colour index
 * @return 0 on success, non-zero on failure
 */
int graphics_draw_rectangle(uint16_t x, uint16_t y,
                             uint16_t width, uint16_t height, uint32_t color);

/**
 * Draws an XPM sprite in the back-buffer with transparency.
 * The XPM colour index 0 is treated as transparent.
 *
 * @param xpm XPM sprite data
 * @param x   Top-left X coordinate
 * @param y   Top-left Y coordinate
 * @return 0 on success, non-zero on failure
 */
int graphics_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);

/**
 * Returns the screen horizontal resolution in pixels.
 *
 * @return Horizontal resolution
 */
uint16_t graphics_get_hres(void);

/**
 * Returns the screen vertical resolution in pixels.
 *
 * @return Vertical resolution
 */
uint16_t graphics_get_vres(void);

/**
 * Returns the number of bytes per scan line.
 *
 * @return Bytes per scan line
 */
unsigned graphics_get_bytes_per_line(void);

/**
 * Returns the number of bytes per pixel.
 *
 * @return Bytes per pixel
 */
unsigned graphics_get_bytes_per_pixel(void);

/**
 * Draws a crosshair cursor centred on the given point.
 * Horizontal and vertical arms extend 7 pixels each with a 5-pixel gap.
 * A 3×3 red dot marks the exact hotspot.
 * Arms are clamped to screen boundaries.
 *
 * @param cx Cursor X coordinate (centre)
 * @param cy Cursor Y coordinate (centre)
 */
void graphics_draw_crosshair(int cx, int cy);

/** @} */

#endif
