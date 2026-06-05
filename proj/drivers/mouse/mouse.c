#include <lcom/lcf.h>

#include <stdint.h>
#include <stdbool.h>

#include "i8042.h"
#include "mouse.h"

static int mouse_hook_id = 2;

static volatile uint8_t ih_byte     = 0;
static volatile bool    ih_has_byte = false;

/* ------------------------------------------------------------------ */
/* Static KBC helpers – ported directly from lab4/mouse.c             */
/* ------------------------------------------------------------------ */

static int kbc_wait_ibf_clear(void) {
  uint8_t status;
  for (int i = 0; i < KBC_MAX_TRIES; i++) {
    if (util_sys_inb(KBC_STAT_REG, &status) != 0) return 1;
    if (!(status & KBC_ST_IBF)) return 0;
    tickdelay(micros_to_ticks(KBC_DELAY_US));
  }
  return 1;
}

static int kbc_read_output(uint8_t *byte, bool expect_aux) {
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

static int kbc_write_cmd(uint8_t cmd) {
  if (kbc_wait_ibf_clear() != 0) return 1;
  if (sys_outb(KBC_CMD_REG, cmd) != 0) return 1;
  return 0;
}

static int kbc_write_arg(uint8_t arg) {
  if (kbc_wait_ibf_clear() != 0) return 1;
  if (sys_outb(KBC_IN_BUF, arg) != 0) return 1;
  return 0;
}

static int kbc_read_cmd_byte(uint8_t *cmd_byte) {
  if (cmd_byte == NULL) return 1;
  if (kbc_write_cmd(KBC_CMD_READ_CMD_BYTE) != 0) return 1;
  return kbc_read_output(cmd_byte, false);
}

static int kbc_write_cmd_byte(uint8_t cb) {
  if (kbc_write_cmd(KBC_CMD_WRITE_CMD_BYTE) != 0) return 1;
  if (kbc_write_arg(cb) != 0) return 1;
  return 0;
}

static int kbc_enable_mouse_ints(void) {
  uint8_t cb;
  if (kbc_read_cmd_byte(&cb) != 0) return 1;
  cb |= KBC_CB_INT_MOUSE;
  return kbc_write_cmd_byte(cb);
}

static int mouse_write_byte(uint8_t byte, uint8_t *ack) {
  if (ack == NULL) return 1;
  if (kbc_write_cmd(KBC_CMD_WRITE_MOUSE) != 0) return 1;
  if (kbc_write_arg(byte) != 0) return 1;
  if (kbc_read_output(ack, true) != 0) return 1;
  return 0;
}

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

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

  /* Prevent stealing keyboard bytes */
  if (!(status & KBC_ST_AUX)) return;

  if (util_sys_inb(KBC_OUT_BUF, &data) != 0) return;

  /* Discard error bytes */
  if (status & KBC_ST_ERR) {
    ih_has_byte = false;
    return;
  }

  ih_byte     = data;
  ih_has_byte = true;
}

bool mouse_has_byte(void) {
  return ih_has_byte;
}

uint8_t mouse_get_byte(void) {
  ih_has_byte = false;
  return ih_byte;
}

int proj_mouse_send_cmd(uint8_t cmd) {
  for (int i = 0; i < KBC_MAX_TRIES; i++) {
    uint8_t ack;
    if (mouse_write_byte(cmd, &ack) != 0) continue;
    if (ack == MOUSE_ACK) return 0;
  }
  return 1;
}

int proj_mouse_disable_reporting(void) {
  return proj_mouse_send_cmd(MOUSE_CMD_DISABLE_DATA_REPORT);
}

int proj_mouse_enable_reporting(void) {
  return proj_mouse_send_cmd(MOUSE_CMD_ENABLE_DATA_REPORT);
}

int proj_mouse_set_stream_mode(void) {
  return proj_mouse_send_cmd(MOUSE_CMD_SET_STREAM_MODE);
}

/**
 * Full initialisation sequence – mirrors lab4's mouse_setup_stream():
 *   1. Disable data reporting
 *   2. Enable AUX port on KBC
 *   3. Enable mouse interrupts in KBC Command Byte
 *   4. Set stream mode
 *   5. Enable data reporting
 */
int mouse_setup_stream(void) {
  if (proj_mouse_disable_reporting() != 0) return 1;
  if (kbc_write_cmd(KBC_CMD_ENABLE_AUX) != 0) return 1;
  if (kbc_enable_mouse_ints()          != 0) return 1;
  if (proj_mouse_set_stream_mode()     != 0) return 1;
  if (proj_mouse_enable_reporting()    != 0) return 1;
  return 0;
}

bool mouse_packet_is_sync(uint8_t byte) {
  return (byte & MOUSE_SYNC_BIT) != 0;
}

bool mouse_packet_is_overflow(const uint8_t packet[3]) {
  return (packet[0] & (MOUSE_X_OV | MOUSE_Y_OV)) != 0;
}

void mouse_packet_parse(const uint8_t packet[3], int16_t *dx, int16_t *dy, bool *lb) {
  *dx = (packet[0] & MOUSE_X_SIGN) ? (int16_t)(0xFF00u | packet[1]) : (int16_t) packet[1];
  *dy = (packet[0] & MOUSE_Y_SIGN) ? (int16_t)(0xFF00u | packet[2]) : (int16_t) packet[2];
  *lb = (packet[0] & MOUSE_LB) != 0;
}
