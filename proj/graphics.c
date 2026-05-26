#include <lcom/lcf.h>

#include <machine/int86.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "graphics.h"

static uint8_t *video_mem = NULL;
static vbe_mode_info_t vmi;

static uint16_t hres;
static uint16_t vres;
static unsigned bytes_per_pixel;
static unsigned bytes_per_line;

static int video_set_mode(uint16_t mode) {
  reg86_t r;
  memset(&r, 0, sizeof(r));

  r.intno = 0x10;
  r.ax = 0x4F02;
  r.bx = (mode & ~BIT(15)) | BIT(14);

  if (sys_int86(&r) != 0) return 1;
  if (r.ax != 0x004F) return 1;

  return 0;
}

static int map_vram(uint16_t mode) {
  if (vbe_get_mode_info(mode, &vmi) != 0) return 1;

  hres = vmi.XResolution;
  vres = vmi.YResolution;
  bytes_per_pixel = (vmi.BitsPerPixel + 7) / 8;
  bytes_per_line = vmi.BytesPerScanLine;

  if (bytes_per_line == 0) {
    bytes_per_line = hres * bytes_per_pixel;
  }

  size_t vram_size = (size_t) bytes_per_line * vres;

  struct minix_mem_range mr;
  mr.mr_base = (phys_bytes) vmi.PhysBasePtr;
  mr.mr_limit = mr.mr_base + vram_size;

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != 0) return 1;

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED) {
    video_mem = NULL;
    return 1;
  }

  return 0;
}

int graphics_init(uint16_t mode) {
  if (map_vram(mode) != 0) return 1;
  if (video_set_mode(mode) != 0) return 1;

  return 0;
}

int graphics_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  if (video_mem == NULL) return 1;
  if (x >= hres || y >= vres) return 1;

  size_t offset = (size_t) y * bytes_per_line + (size_t) x * bytes_per_pixel;

  for (unsigned i = 0; i < bytes_per_pixel; i++) {
    video_mem[offset + i] = (uint8_t) (color >> (8 * i));
  }

  return 0;
}

int graphics_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  if (len == 0) return 0;
  if (x >= hres || y >= vres) return 1;
  if ((uint32_t) x + len > hres) return 1;

  for (uint16_t i = 0; i < len; i++) {
    if (graphics_draw_pixel(x + i, y, color) != 0) return 1;
  }

  return 0;
}

int graphics_draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  if (width == 0 || height == 0) return 0;
  if (x >= hres || y >= vres) return 1;
  if ((uint32_t) x + width > hres) return 1;
  if ((uint32_t) y + height > vres) return 1;

  for (uint16_t row = 0; row < height; row++) {
    if (graphics_draw_hline(x, y + row, width, color) != 0) return 1;
  }

  return 0;
}