// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <machine/int86.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv)) return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

#define KBD_IRQ 1
#define KBC_OUT_BUF 0x60
#define KBC_STAT_REG 0x64

#define KBC_ST_OBF BIT(0)
#define KBC_ST_AUX BIT(5)
#define KBC_ST_ERR (BIT(6) | BIT(7))

#define KBD_ESC_BREAK 0x81

static uint8_t *video_mem = NULL;
static vbe_mode_info_t vmi;
static uint16_t hres;
static uint16_t vres;
static unsigned bytes_per_pixel;
static unsigned bytes_per_line;

static int kbd_hook_id = 1;
static volatile uint8_t kbd_byte = 0;
static volatile bool kbd_has_data = false;

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t data;
  if (value == NULL) return -1;
  if (sys_inb(port, &data) != 0) return -1;
  *value = (uint8_t)(data & 0xFF);
  return 0;
}

static int kbd_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;

  *bit_no = (uint8_t) kbd_hook_id;
  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != 0) return 1;

  return 0;
}

static int kbd_unsubscribe_int(void) {
  if (sys_irqrmpolicy(&kbd_hook_id) != 0) return 1;
  return 0;
}

static void kbd_ih(void) {
  uint8_t status;
  uint8_t data;

  if (util_sys_inb(KBC_STAT_REG, &status) != 0) return;
  if (!(status & KBC_ST_OBF)) return;
  if (util_sys_inb(KBC_OUT_BUF, &data) != 0) return;

  if (status & (KBC_ST_AUX | KBC_ST_ERR)) {
    kbd_has_data = false;
    return;
  }

  kbd_byte = data;
  kbd_has_data = true;
}

static int wait_for_esc(void) {
  uint8_t kbd_bit_no;
  if (kbd_subscribe_int(&kbd_bit_no) != 0) return 1;

  uint32_t irq_set = BIT(kbd_bit_no);
  int ipc_status;
  message msg;

  while (true) {
    int r = driver_receive(ANY, &msg, &ipc_status);
    if (r != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & irq_set) {
          kbd_ih();

          if (kbd_has_data) {
            kbd_has_data = false;
            if (kbd_byte == KBD_ESC_BREAK) {
              return kbd_unsubscribe_int();
            }
          }
        }
      }
    }
  }
}

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
  mr.mr_base = (phys_bytes) vmi.PhysBasePtr; //endereço físico do início da memória de vídeo
  mr.mr_limit = mr.mr_base + vram_size;


//pede permição ao kernel para mapear a memória do VRAM no espaço de endereçamento do processo
  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != 0) return 1;


//mapeia a memória do VRAM no espaço de endereçamento do processo
  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED) {
    video_mem = NULL;
    return 1;
  }

  return 0;
}

static int draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  if (video_mem == NULL) return 1;
  if (x >= hres || y >= vres) return 1;

  size_t offset = (size_t) y * bytes_per_line + (size_t) x * bytes_per_pixel;
  for (unsigned i = 0; i < bytes_per_pixel; i++) {
    video_mem[offset + i] = (uint8_t) (color >> (8 * i));
  }

  return 0;
}

static int draw_hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  if (len == 0) return 0;
  if (x >= hres || y >= vres) return 1;
  if ((uint32_t) x + len > hres) return 1;

  for (uint16_t i = 0; i < len; i++) {
    if (draw_pixel(x + i, y, color) != 0) return 1;
  }

  return 0;
}

static int draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  if (width == 0 || height == 0) return 0;
  if (x >= hres || y >= vres) return 1;
  if ((uint32_t) x + width > hres) return 1;
  if ((uint32_t) y + height > vres) return 1;

  for (uint16_t row = 0; row < height; row++) {
    if (draw_hline(x, y + row, width, color) != 0) return 1;
  }

  return 0;
}

static int draw_pixmap(const uint8_t *pixmap, const xpm_image_t *img, uint16_t x, uint16_t y) {
  if (video_mem == NULL || pixmap == NULL || img == NULL) return 1;
  if ((uint32_t) x + img->width > hres) return 1;
  if ((uint32_t) y + img->height > vres) return 1;

  size_t pixels = (size_t) img->width * img->height;
  if (pixels == 0) return 1;
  size_t pix_bytes = img->size / pixels;
  if (pix_bytes == 0 || pix_bytes != bytes_per_pixel) return 1;

  for (uint16_t row = 0; row < img->height; row++) {
    size_t src_off = (size_t) row * img->width * pix_bytes;
    size_t dst_off = (size_t) (y + row) * bytes_per_line + (size_t) x * bytes_per_pixel;
    memcpy(video_mem + dst_off, pixmap + src_off, img->width * pix_bytes);
  }

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (video_set_mode(mode) != 0) return 1;
  sleep(delay);
  if (vg_exit() != 0) return 1;

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  if (map_vram(mode) != 0) return 1;
  if (video_set_mode(mode) != 0) return 1;

  int ret = 0;

  if (draw_rectangle(x, y, width, height, color) != 0) {
    ret = 1;
  }
  else if (wait_for_esc() != 0) {
    ret = 1;
  }

  if (vg_exit() != 0) ret = 1;
  return ret;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  const uint16_t mode = 0x105;

  if (map_vram(mode) != 0) return 1;
  if (video_set_mode(mode) != 0) return 1;

  xpm_image_t img;
  uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &img);
  if (pixmap == NULL) {
    (void) vg_exit();
    return 1;
  }

  int ret = 0;

  if (draw_pixmap(pixmap, &img, x, y) != 0) {
    ret = 1;
  }
  free(pixmap);

  if (ret == 0 && wait_for_esc() != 0) {
    ret = 1;
  }

  if (vg_exit() != 0) ret = 1;
  return ret;
}
