#include <lcom/lcf.h>

#include <stdint.h>
#include <stdbool.h>

#include "i8042.h"
#include "keyboard.h"

#define SCANCODE_EXTENDED 0xE0

static int     kbd_hook_id       = 1;
static volatile uint8_t kbd_byte = 0;
static volatile bool kbd_data_available = false;

/* Set when 0xE0 extended prefix has been received; next byte is the real code. */
static volatile bool kbd_extended = false;

/**
 * Utility wrapper around sys_inb – returns an 8-bit value.
 * Defined here so both keyboard and mouse modules share the same definition.
 * LCF wraps this function for grading (hence the parentheses around the name).
 */
int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t data;
  if (value == NULL) return 1;
  if (sys_inb(port, &data) != 0) return 1;
  *value = (uint8_t)(data & 0xFF);
  return 0;
}

int keyboard_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = (uint8_t) kbd_hook_id;
  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != 0) return 1;
  return 0;
}

int keyboard_unsubscribe_int(void) {
  if (sys_irqrmpolicy(&kbd_hook_id) != 0) return 1;
  return 0;
}

void keyboard_ih(void) {
  uint8_t status;
  uint8_t data;

  if (util_sys_inb(KBC_STAT_REG, &status) != 0) return;
  if (!(status & KBC_ST_OBF)) return;

  /* Prevent stealing mouse bytes */
  if (status & KBC_ST_AUX) return;

  if (util_sys_inb(KBC_OUT_BUF, &data) != 0) return;

  /* Discard error bytes */
  if (status & KBC_ST_ERR) {
    kbd_data_available = false;
    return;
  }

  /* Swallow the 0xE0 extended prefix; mark the next byte as extended. */
  if (data == SCANCODE_EXTENDED) {
    kbd_extended      = true;
    kbd_data_available = false;
    return;
  }

  kbd_byte           = data;
  kbd_data_available = true;
  kbd_extended       = false; /* consumed */
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