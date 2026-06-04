
#include <lcom/lcf.h>
#include <machine/int86.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "graphics.h"

/* ---- Module state ---- */
static uint8_t        *video_mem   = NULL;  /**< Mapped VRAM (write-only during flush) */
static uint8_t        *back_buffer = NULL;  /**< Off-screen draw surface */
static vbe_mode_info_t vmi;

static uint16_t hres;
static uint16_t vres;
static unsigned bytes_per_pixel;
static unsigned bytes_per_line;

static int video_set_mode(uint16_t mode) {
  reg86_t r;
  memset(&r, 0, sizeof(r));
  r.intno = 0x10;
  r.ax    = 0x4F02;
  r.bx    = (mode & ~BIT(15)) | BIT(14);  /* linear frame buffer */
  if (sys_int86(&r) != 0) return 1;
  if (r.ax != 0x004F)     return 1;
  return 0;
}

static int map_vram(uint16_t mode) {
  if (vbe_get_mode_info(mode, &vmi) != 0) return 1;

  hres            = vmi.XResolution;
  vres            = vmi.YResolution;
  bytes_per_pixel = (vmi.BitsPerPixel + 7) / 8;
  bytes_per_line  = vmi.BytesPerScanLine;
  if (bytes_per_line == 0)
    bytes_per_line = hres * bytes_per_pixel;

  size_t vram_size = (size_t) bytes_per_line * vres;

  struct minix_mem_range mr;
  mr.mr_base  = (phys_bytes) vmi.PhysBasePtr;
  mr.mr_limit = mr.mr_base + vram_size;

  /* Grant kernel permission to access physical VRAM */
  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != 0) return 1;

  /* Map VRAM into process address space */
  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED) { video_mem = NULL; return 1; }

  return 0;
}

/* ---- Public API ---- */

int graphics_init(uint16_t mode) {
  if (map_vram(mode)       != 0) return 1;
  if (video_set_mode(mode) != 0) return 1;

  size_t sz  = (size_t) bytes_per_line * vres;
  back_buffer = (uint8_t *) malloc(sz);
  if (back_buffer == NULL) return 1;

  memset(back_buffer, 0, sz);
  return 0;
}

void graphics_clear(uint32_t color) {
  if (back_buffer == NULL) return;
  size_t sz = (size_t) bytes_per_line * vres;

  if (bytes_per_pixel == 1) {
    /* Fast path: indexed 8-bit – one byte per pixel */
    memset(back_buffer, (int)(color & 0xFF), sz);
    return;
  }
  /* Generic path: multi-byte pixels */
  for (uint16_t y = 0; y < vres; y++) {
    for (uint16_t x = 0; x < hres; x++) {
      size_t off = (size_t) y * bytes_per_line + (size_t) x * bytes_per_pixel;
      for (unsigned i = 0; i < bytes_per_pixel; i++)
        back_buffer[off + i] = (uint8_t)(color >> (8 * i));
    }
  }
}

void graphics_flush(void) {
  if (video_mem == NULL || back_buffer == NULL) return;
  memcpy(video_mem, back_buffer, (size_t) bytes_per_line * vres);
}

int graphics_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  if (back_buffer == NULL)    return 1;
  if (x >= hres || y >= vres) return 1;

  size_t off = (size_t) y * bytes_per_line + (size_t) x * bytes_per_pixel;
  for (unsigned i = 0; i < bytes_per_pixel; i++)
    back_buffer[off + i] = (uint8_t)(color >> (8 * i));

  return 0;
}

int graphics_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  if (len == 0) return 0;
  if (x >= hres || y >= vres) return 1;
  if ((uint32_t) x + len > hres) return 1;

  for (unsigned int i = 0; i < len; i++) {
    unsigned int cx = x + i;
    if (graphics_draw_pixel((uint16_t) cx, y, color) != 0) return 1;
  }
  return 0;
}

int graphics_draw_rectangle(uint16_t x, uint16_t y,
                             uint16_t width, uint16_t height, uint32_t color) {
  if (width == 0 || height == 0) return 0;
  if (x >= hres || y >= vres)    return 1;
  if ((uint32_t) x + width  > hres) return 1;
  if ((uint32_t) y + height > vres) return 1;

  for (unsigned int row = 0; row < height; row++) {
    unsigned int cy = y + row;
    if (graphics_draw_hline(x, (uint16_t) cy, width, color) != 0) return 1;
  }
  return 0;
}

/**
 * Draw an XPM sprite at (x, y) with transparency (colour 0 = transparent).
 * Uses the same draw_pixmap approach from lab5.
 */
int graphics_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (xpm == NULL || back_buffer == NULL) return 1;

  xpm_image_t img;
  uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &img);
  if (pixmap == NULL) return 1;

  int ret = 0;
  if ((uint32_t) x + img.width  > hres ||
      (uint32_t) y + img.height > vres) {
    ret = 1;
    goto cleanup;
  }

  for (uint16_t row = 0; row < img.height; row++) {
    for (uint16_t col = 0; col < img.width; col++) {
      size_t src_off = ((size_t) row * img.width + col) * bytes_per_pixel;

      /* Colour 0 = transparent in XPM_INDEXED mode */
      bool transparent = true;
      for (unsigned b = 0; b < bytes_per_pixel; b++) {
        if (pixmap[src_off + b] != 0) { transparent = false; break; }
      }
      if (transparent) continue;

      size_t dst_off = (size_t)(y + row) * bytes_per_line +
                       (size_t)(x + col) * bytes_per_pixel;
      for (unsigned b = 0; b < bytes_per_pixel; b++)
        back_buffer[dst_off + b] = pixmap[src_off + b];
    }
  }

cleanup:
  free(pixmap);
  return ret;
}

uint16_t graphics_get_hres(void)           { return hres; }
uint16_t graphics_get_vres(void)           { return vres; }
unsigned graphics_get_bytes_per_line(void) { return bytes_per_line; }
unsigned graphics_get_bytes_per_pixel(void){ return bytes_per_pixel; }

void graphics_draw_crosshair(int cx, int cy) {
  /* Horizontal arms — clamp to screen edges */
  int lx = cx - 10;
  int rx = cx + 4;
  if (lx < 0) lx = 0;
  if (rx + 6 >= (int) hres) rx = (int) hres - 7;

  graphics_draw_hline((uint16_t) lx,      (uint16_t) cy,
                      (uint16_t)(cx - 3 - lx),  0x0Fu);
  graphics_draw_hline((uint16_t)(cx + 4), (uint16_t) cy,
                      (uint16_t)(rx - cx - 3),  0x0Fu);

  /* Vertical arms */
  for (int dy = -10; dy <= 10; dy++) {
    if (dy >= -2 && dy <= 2) continue;
    int py = cy + dy;
    if (py >= 0 && py < (int) vres)
      graphics_draw_pixel((uint16_t) cx, (uint16_t) py, 0x0Fu);
  }

  /* Centre dot */
  graphics_draw_rectangle((uint16_t)(cx - 1), (uint16_t)(cy - 1), 3, 3, 0x0Cu);
}
