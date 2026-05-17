#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include <sys/types.h>

#ifndef __minix
typedef uint32_t u32_t;
#endif

#include "i8042.h"
#include "mouse.h"

static int mouse_hook_id = 2;

static volatile uint8_t ih_byte = 0;
static volatile bool ih_has_byte = false;

static int (kbc_wait_ibf_clear)(void) {
  uint8_t status;

  for (int i = 0; i < KBC_MAX_TRIES; i++) {
    if (util_sys_inb(KBC_STAT_REG, &status) != 0) return 1;
    if (!(status & KBC_ST_IBF)) return 0;
    tickdelay(micros_to_ticks(KBC_DELAY_US));
  }

  return 1;
}

static int (kbc_read_output)(uint8_t *byte, bool expect_aux) {
  if (byte == NULL) return 1;

  for (int i = 0; i < KBC_MAX_TRIES; i++) {
    uint8_t status;
    if (util_sys_inb(KBC_STAT_REG, &status) != 0) return 1;

    if (status & KBC_ST_OBF) {
      uint8_t data;
      if (util_sys_inb(KBC_OUT_BUF, &data) != 0) return 1;

      if (status & KBC_ST_ERR) {
        tickdelay(micros_to_ticks(KBC_DELAY_US));
        continue;
      }

      if (expect_aux && !(status & KBC_ST_AUX)) {
        tickdelay(micros_to_ticks(KBC_DELAY_US));
        continue;
      }

      if (!expect_aux && (status & KBC_ST_AUX)) {
        tickdelay(micros_to_ticks(KBC_DELAY_US));
        continue;
      }

      *byte = data;
      return 0;
    }

    tickdelay(micros_to_ticks(KBC_DELAY_US));
  }

  return 1;
}

static int (kbc_write_cmd)(uint8_t cmd) {
  if (kbc_wait_ibf_clear() != 0) return 1;
  if (sys_outb(KBC_CMD_REG, cmd) != 0) return 1;
  return 0;
}

static int (kbc_write_arg)(uint8_t arg) {
  if (kbc_wait_ibf_clear() != 0) return 1;
  if (sys_outb(KBC_IN_BUF, arg) != 0) return 1;
  return 0;
}

int (mouse_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;

  *bit_no = (uint8_t) mouse_hook_id;

  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != 0) return 1;

  return 0;
}

int (mouse_unsubscribe_int)(void) {
  if (sys_irqrmpolicy(&mouse_hook_id) != 0) return 1;
  return 0;
}

void (mouse_ih)(void) {
  uint8_t status;
  uint8_t data;

  if (util_sys_inb(KBC_STAT_REG, &status) != 0) return;

  if (!(status & KBC_ST_OBF)) return;

  if (util_sys_inb(KBC_OUT_BUF, &data) != 0) return;

  if (!(status & KBC_ST_AUX) || (status & KBC_ST_ERR)) {
    ih_has_byte = false;
    return;
  }

  ih_byte = data;
  ih_has_byte = true;
}

bool (mouse_has_byte)(void) {
  return ih_has_byte;
}

uint8_t (mouse_get_byte)(void) {
  ih_has_byte = false;
  return ih_byte;
}

static int (mouse_write_byte)(uint8_t byte, uint8_t *ack) {
  if (ack == NULL) return 1;

  if (kbc_write_cmd(KBC_CMD_WRITE_MOUSE) != 0) return 1;
  if (kbc_write_arg(byte) != 0) return 1;
  if (kbc_read_output(ack, true) != 0) return 1;

  return 0;
}

int (mouse_send_cmd)(uint8_t cmd) {
  for (int i = 0; i < KBC_MAX_TRIES; i++) {
    uint8_t ack;
    if (mouse_write_byte(cmd, &ack) != 0) continue;
    if (ack == MOUSE_ACK) return 0;
    if (ack == MOUSE_NACK || ack == MOUSE_ERROR) continue;
  }

  return 1;
}

int (mouse_send_cmd_arg)(uint8_t cmd, uint8_t arg) {
  for (int i = 0; i < KBC_MAX_TRIES; i++) {
    uint8_t ack;

    if (mouse_write_byte(cmd, &ack) != 0) continue;
    if (ack != MOUSE_ACK) continue;

    if (mouse_write_byte(arg, &ack) != 0) continue;
    if (ack == MOUSE_ACK) return 0;
  }

  return 1;
}

int (mouse_disable_data_reporting)(void) {
  return mouse_send_cmd(MOUSE_CMD_DISABLE_DATA_REPORT);
}

int (mouse_enable_data_reporting_local)(void) {
  return mouse_send_cmd(MOUSE_CMD_ENABLE_DATA_REPORT);
}

int (mouse_set_stream_mode)(void) {
  return mouse_send_cmd(MOUSE_CMD_SET_STREAM_MODE);
}

int (mouse_set_remote_mode)(void) {
  return mouse_send_cmd(MOUSE_CMD_SET_REMOTE_MODE);
}
