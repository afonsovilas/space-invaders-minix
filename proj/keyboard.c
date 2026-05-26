#include <lcom/lcf.h>

#include <stdint.h>
#include <stdbool.h>

#include "keyboard.h"

#define KBD_IRQ 1

#define KBC_OUT_BUF 0x60
#define KBC_STAT_REG 0x64

#define KBC_ST_OBF BIT(0)
#define KBC_ST_AUX BIT(5)
#define KBC_ST_ERR (BIT(6) | BIT(7))

static int kbd_hook_id = 1;
static volatile uint8_t kbd_byte = 0;
static volatile bool kbd_data_available = false;

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t data;

  if (value == NULL) return 1;

  if (sys_inb(port, &data) != 0) return 1;

  *value = (uint8_t)(data & 0xFF);

  return 0;
}

int keyboard_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;

  *bit_no = (uint8_t) kbd_hook_id;

  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != 0) {
    return 1;
  }

  return 0;
}

int keyboard_unsubscribe_int(void) {
  if (sys_irqrmpolicy(&kbd_hook_id) != 0) {
    return 1;
  }

  return 0;
}

void keyboard_ih(void) {
  uint8_t status;
  uint8_t data;

  if (util_sys_inb(KBC_STAT_REG, &status) != 0) return;

  if (!(status & KBC_ST_OBF)) return;

  if (util_sys_inb(KBC_OUT_BUF, &data) != 0) return;

  if (status & (KBC_ST_AUX | KBC_ST_ERR)) {
    kbd_data_available = false;
    return;
  }

  kbd_byte = data;
  kbd_data_available = true;
}

bool keyboard_has_data(void) {
  return kbd_data_available;
}

uint8_t keyboard_get_byte(void) {
  return kbd_byte;
}

void keyboard_clear_data(void) {
  kbd_data_available = false;
}