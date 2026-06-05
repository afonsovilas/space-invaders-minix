#include <lcom/lcf.h>

#include <stdint.h>
#include <stdbool.h>

#include "i8042.h"
#include "keyboard.h"

#define SCANCODE_EXTENDED 0xE0

/* ------------------------------------------------------------------ */
/* Scancode → uppercase letter (make-codes 0x00–0x7F)                 */
/* '\0' means the key does not map to a letter.                        */
/* ------------------------------------------------------------------ */

const char kbd_scancode_to_char[128] = {
  /* 0x00 */ '\0',
  /* 0x01 */ '\0', /* ESC make      */
  /* 0x02 */ '\0', /* 1             */
  /* 0x03 */ '\0', /* 2             */
  /* 0x04 */ '\0', /* 3             */
  /* 0x05 */ '\0', /* 4             */
  /* 0x06 */ '\0', /* 5             */
  /* 0x07 */ '\0', /* 6             */
  /* 0x08 */ '\0', /* 7             */
  /* 0x09 */ '\0', /* 8             */
  /* 0x0A */ '\0', /* 9             */
  /* 0x0B */ '\0', /* 0             */
  /* 0x0C */ '\0', /* -             */
  /* 0x0D */ '\0', /* =             */
  /* 0x0E */ '\0', /* BACKSPACE     */
  /* 0x0F */ '\0', /* TAB           */
  /* 0x10 */ 'Q',
  /* 0x11 */ 'W',
  /* 0x12 */ 'E',
  /* 0x13 */ 'R',
  /* 0x14 */ 'T',
  /* 0x15 */ 'Y',
  /* 0x16 */ 'U',
  /* 0x17 */ 'I',
  /* 0x18 */ 'O',
  /* 0x19 */ 'P',
  /* 0x1A */ '\0', /* [             */
  /* 0x1B */ '\0', /* ]             */
  /* 0x1C */ '\0', /* ENTER         */
  /* 0x1D */ '\0', /* CTRL          */
  /* 0x1E */ 'A',
  /* 0x1F */ 'S',
  /* 0x20 */ 'D',
  /* 0x21 */ 'F',
  /* 0x22 */ 'G',
  /* 0x23 */ 'H',
  /* 0x24 */ 'J',
  /* 0x25 */ 'K',
  /* 0x26 */ 'L',
  /* 0x27 */ '\0', /* ;             */
  /* 0x28 */ '\0', /* '             */
  /* 0x29 */ '\0', /* `             */
  /* 0x2A */ '\0', /* LSHIFT        */
  /* 0x2B */ '\0', /* \             */
  /* 0x2C */ 'Z',
  /* 0x2D */ 'X',
  /* 0x2E */ 'C',
  /* 0x2F */ 'V',
  /* 0x30 */ 'B',
  /* 0x31 */ 'N',
  /* 0x32 */ 'M',
  /* 0x33–0x7F: all '\0' */
  '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0',
};

static int     kbd_hook_id       = 1;
static volatile uint8_t kbd_byte = 0;
static volatile bool kbd_data_available = false;

/* Set when 0xE0 extended prefix has been received; next byte is the real code. */
static volatile bool kbd_extended = false;

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

void keyboard_flush_buffers(void) {
  uint8_t stat, data;
  while (util_sys_inb(KBC_STAT_REG, &stat) == 0 && (stat & KBC_ST_OBF)) {
    util_sys_inb(KBC_OUT_BUF, &data);
    tickdelay(micros_to_ticks(KBC_DELAY_US));
  }
}