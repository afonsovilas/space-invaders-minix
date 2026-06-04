#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <lcom/lcf.h>

/**
 * @defgroup graphics Graphics
 * VBE mode 0x105 (8-bit indexed, 1024×768) with double-buffering.
 *
 * Frame pattern:
 *   graphics_clear(0x00);      // wipe back-buffer
 *   // ... draw calls ...
 *   graphics_flush();          // blit back-buffer → VRAM (no flicker)
 * @{
 */

/** Initialise VRAM and allocate the back-buffer. Must be called once. */
int  graphics_init(uint16_t mode);

/** Fill the entire back-buffer with a single colour index. */
void graphics_clear(uint32_t color);

/** Copy the back-buffer to VRAM (double-buffer swap). */
void graphics_flush(void);

/** Draw a single pixel into the back-buffer. */
int graphics_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/** Draw a horizontal line into the back-buffer. */
int graphics_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/** Draw a filled rectangle into the back-buffer. */
int graphics_draw_rectangle(uint16_t x, uint16_t y,
                             uint16_t width, uint16_t height, uint32_t color);

/** Load an XPM and blit it into the back-buffer at (x, y) with transparency. */
int graphics_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);

/** Return horizontal resolution. */
uint16_t graphics_get_hres(void);

/** Return vertical resolution. */
uint16_t graphics_get_vres(void);

/** Return bytes per scan line. */
unsigned graphics_get_bytes_per_line(void);

/** Return bytes per pixel. */
unsigned graphics_get_bytes_per_pixel(void);

#endif
