#include <lcom/lcf.h>

#include <lcom/lab4.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include <sys/types.h>

#ifndef __minix
typedef uint32_t u32_t;
#endif

#include "i8042.h"
#include "mouse.h"


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

static int (kbc_read_cmd_byte)(uint8_t *cmd_byte) {
  if (cmd_byte == NULL) return 1;
  if (kbc_write_cmd(KBC_CMD_READ_CMD_BYTE) != 0) return 1;
  return kbc_read_output(cmd_byte, false);
}

static int (kbc_write_cmd_byte)(uint8_t cmd_byte) {
  if (kbc_write_cmd(KBC_CMD_WRITE_CMD_BYTE) != 0) return 1;
  if (kbc_write_arg(cmd_byte) != 0) return 1;
  return 0;
}

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

static int16_t (sign_extend)(uint8_t value, bool sign) {
  if (sign) return (int16_t)(0xFF00 | value);
  return (int16_t) value;
}

static void (mouse_parse_packet)(const uint8_t bytes[3], struct packet *pp) {
  pp->bytes[0] = bytes[0];
  pp->bytes[1] = bytes[1];
  pp->bytes[2] = bytes[2];

  pp->lb = bytes[0] & MOUSE_LB;
  pp->rb = bytes[0] & MOUSE_RB;
  pp->mb = bytes[0] & MOUSE_MB;

  pp->x_ov = bytes[0] & MOUSE_X_OV;
  pp->y_ov = bytes[0] & MOUSE_Y_OV;

  pp->delta_x = sign_extend(bytes[1], bytes[0] & MOUSE_X_SIGN);
  pp->delta_y = sign_extend(bytes[2], bytes[0] & MOUSE_Y_SIGN);
}

static bool (mouse_push_byte)(uint8_t byte, uint8_t bytes[3], uint8_t *idx) {
  if (*idx == 0 && !(byte & MOUSE_SYNC_BIT)) return false;

  bytes[*idx] = byte;
  (*idx)++;

  if (*idx == 3) {
    *idx = 0;
    return true;
  }

  return false;
}

static int (kbc_enable_mouse_ints)(void) {
  uint8_t cmd_byte;
  if (kbc_read_cmd_byte(&cmd_byte) != 0) return 1;
  cmd_byte |= KBC_CB_INT_MOUSE;
  return kbc_write_cmd_byte(cmd_byte);
}

static int (mouse_setup_stream)(void) {
  if (mouse_disable_data_reporting() != 0) return 1;
  if (kbc_write_cmd(KBC_CMD_ENABLE_AUX) != 0) return 1;
  if (kbc_enable_mouse_ints() != 0) return 1;
  if (mouse_set_stream_mode() != 0) return 1;
  if (mouse_enable_data_reporting_local() != 0) return 1;
  return 0;
}

static int (mouse_poll_packet)(uint8_t bytes[3]) {
  uint8_t idx = 0;

  while (idx < 3) {
    uint8_t byte;
    if (kbc_read_output(&byte, true) != 0) return 1;
    if (idx == 0 && !(byte & MOUSE_SYNC_BIT)) continue;
    bytes[idx] = byte;
    idx++;
  }

  return 0;
}

static struct mouse_ev *(mouse_detect_event_local)(struct packet *pp) {
  static struct mouse_ev ev;
  static bool lb_pressed = false;
  static bool rb_pressed = false;

  ev.delta_x = pp->delta_x;
  ev.delta_y = pp->delta_y;

  if (pp->lb && !lb_pressed) {
    ev.type = LB_PRESSED;
    lb_pressed = true;
  }
  else if (!pp->lb && lb_pressed) {
    ev.type = LB_RELEASED;
    lb_pressed = false;
  }
  else if (pp->rb && !rb_pressed) {
    ev.type = RB_PRESSED;
    rb_pressed = true;
  }
  else if (!pp->rb && rb_pressed) {
    ev.type = RB_RELEASED;
    rb_pressed = false;
  }
  else if (pp->delta_x != 0 || pp->delta_y != 0) {
    ev.type = MOUSE_MOV;
  }
  else {
    ev.type = BUTTON_EV;
  }

  return &ev;
}

typedef enum {
  GESTURE_INIT,
  GESTURE_DRAW,
  GESTURE_DONE
} gesture_state_t;

static uint16_t (iabs16)(int16_t value) {
  return (value < 0) ? (uint16_t)(-value) : (uint16_t)value;
}

static void (gesture_update)(gesture_state_t *state, struct mouse_ev *ev,
                             uint8_t x_len, uint8_t tolerance,
                             int16_t *x_sum, int16_t *y_sum) {
  switch (*state) {
    case GESTURE_INIT:
      if (ev->type == LB_PRESSED) {
        *state = GESTURE_DRAW;
        *x_sum = 0;
        *y_sum = 0;
      }
      break;

    case GESTURE_DRAW:
      if (ev->type == RB_PRESSED || ev->type == RB_RELEASED) {
        *state = GESTURE_INIT;
        *x_sum = 0;
        *y_sum = 0;
        break;
      }

      if (ev->type == LB_RELEASED) {
        if (*x_sum >= (int16_t) x_len) *state = GESTURE_DONE;
        else *state = GESTURE_INIT;
        break;
      }

      if (ev->type == MOUSE_MOV) {
        if (ev->delta_x >= 0 && ev->delta_y >= 0) {
          *x_sum += ev->delta_x;
          *y_sum += ev->delta_y;
          if (*x_sum >= (int16_t) x_len) *state = GESTURE_DONE;
        }
        else if (iabs16(ev->delta_x) > tolerance || iabs16(ev->delta_y) > tolerance) {
          *state = GESTURE_INIT;
          *x_sum = 0;
          *y_sum = 0;
        }
      }
      break;

    case GESTURE_DONE:
      break;
  }
}

int (mouse_test_packet)(uint32_t cnt) {
  uint8_t mouse_bit_no;
  if (mouse_subscribe_int(&mouse_bit_no) != 0) return 1;

  int ret = 0;
  if (mouse_setup_stream() != 0) {
    ret = 1;
    goto cleanup;
  }

  uint32_t irq_set = BIT(mouse_bit_no);
  uint8_t bytes[3];
  uint8_t idx = 0;
  uint32_t packets = 0;

  int ipc_status;
  message msg;

  while (packets < cnt) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();

            if (!mouse_has_byte()) break;
            uint8_t byte = mouse_get_byte();

            if (mouse_push_byte(byte, bytes, &idx)) {
              struct packet pp;
              mouse_parse_packet(bytes, &pp);
              mouse_print_packet(&pp);
              packets++;
            }
          }
          break;

        default:
          break;
      }
    }
  }

cleanup:
  (void) mouse_disable_data_reporting();
  (void) mouse_unsubscribe_int();
  return ret;
}

/*int (mouse_test_async)(uint8_t idle_time) {
  if (timer_counter == UINT32_MAX) timer_counter = 0;

  uint8_t mouse_bit_no;
  if (mouse_subscribe_int(&mouse_bit_no) != 0) return 1;

  uint8_t timer_bit_no;
  if (timer_subscribe_int(&timer_bit_no) != 0) {
    (void) mouse_unsubscribe_int();
    return 1;
  }

  int ret = 0;
  if (mouse_setup_stream() != 0) {
    ret = 1;
    goto cleanup;
  }

  uint32_t irq_set_mouse = BIT(mouse_bit_no);
  uint32_t irq_set_timer = BIT(timer_bit_no);

  uint32_t idle_ticks = 0;
  uint32_t ticks_per_sec = (uint32_t) sys_hz();

  uint8_t bytes[3];
  uint8_t idx = 0;

  int ipc_status;
  message msg;

  while (true) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set_timer) {
            timer_int_handler();
            idle_ticks++;
            if (idle_time > 0 && idle_ticks >= (uint32_t) idle_time * ticks_per_sec) {
              goto cleanup;
            }
          }

          if (msg.m_notify.interrupts & irq_set_mouse) {
            mouse_ih();

            if (!mouse_has_byte()) break;
            uint8_t byte = mouse_get_byte();

            if (mouse_push_byte(byte, bytes, &idx)) {
              struct packet pp;
              mouse_parse_packet(bytes, &pp);
              mouse_print_packet(&pp);
              idle_ticks = 0;
            }
          }
          break;

        default:
          break;
      }
    }
  }



cleanup:
  (void) mouse_disable_data_reporting();
  (void) timer_unsubscribe_int();
  (void) mouse_unsubscribe_int();
  return ret;
}
*/
int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  if (mouse_disable_data_reporting() != 0) return 1;
  if (mouse_set_remote_mode() != 0) return 1;

  for (uint8_t i = 0; i < cnt; i++) {
    if (mouse_send_cmd(MOUSE_CMD_READ_DATA) != 0) return 1;

    uint8_t bytes[3];
    if (mouse_poll_packet(bytes) != 0) return 1;

    struct packet pp;
    mouse_parse_packet(bytes, &pp);
    mouse_print_packet(&pp);

    tickdelay(micros_to_ticks((uint32_t) period * 1000));
  }

  if (mouse_set_stream_mode() != 0) return 1;
  if (mouse_enable_data_reporting_local() != 0) return 1;

  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  uint8_t mouse_bit_no;
  if (mouse_subscribe_int(&mouse_bit_no) != 0) return 1;

  int ret = 0;
  if (mouse_setup_stream() != 0) {
    ret = 1;
    goto cleanup;
  }

  uint32_t irq_set = BIT(mouse_bit_no);

  uint8_t bytes[3];
  uint8_t idx = 0;

  gesture_state_t state = GESTURE_INIT;
  int16_t x_sum = 0;
  int16_t y_sum = 0;

  int ipc_status;
  message msg;

  while (state != GESTURE_DONE) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();

            if (!mouse_has_byte()) break;
            uint8_t byte = mouse_get_byte();

            if (mouse_push_byte(byte, bytes, &idx)) {
              struct packet pp;
              mouse_parse_packet(bytes, &pp);
              mouse_print_packet(&pp);

              struct mouse_ev *ev = mouse_detect_event_local(&pp);
              gesture_update(&state, ev, x_len, tolerance, &x_sum, &y_sum);
            }
          }
          break;

        default:
          break;
      }
    }
  }

cleanup:
  (void) mouse_disable_data_reporting();
  (void) mouse_unsubscribe_int();
  return ret;
}
