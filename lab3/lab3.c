#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "kbc.h"

extern uint32_t util_sys_inb_counter;
extern volatile unsigned int timer_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t kbd_bit_no;
  if (kbd_subscribe_int(&kbd_bit_no) != 0) return 1;

  uint32_t irq_set = BIT(kbd_bit_no);

  uint8_t bytes[2];
  uint8_t size = 0;

  int ipc_status;
  message msg;

  bool done = false;

  while (!done) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();

            if (!kbc_has_byte()) break;
            uint8_t b = kbc_get_byte();

            if (size == 0 && b == 0xE0) {
              bytes[0] = b;
              size = 1;
              break;
            }

            if (size == 1 && bytes[0] == 0xE0) {
              bytes[1] = b;
              size = 2;
            }
            else {
              bytes[0] = b;
              size = 1;
            }

            bool make = !(bytes[size - 1] & BIT(7));
            if (kbd_print_scancode(make, size, bytes) != 0) {
              (void) kbd_unsubscribe_int();
              return 1;
            }

            if (size == 1 && bytes[0] == KBD_ESC_BREAK) {
              done = true;
              break;
            }
            size = 0;
          }
          break;

        default:
          break;
      }
    }
  }

  return kbd_unsubscribe_int();
}

int(kbd_test_poll)() {
  util_sys_inb_counter = 0;

  uint8_t bytes[2];
  uint8_t size = 0;

  while (true) {
    uint8_t b;
    if (kbc_read_byte(&b) != 0) continue;

    if (size == 0 && b == 0xE0) {
      bytes[0] = b;
      size = 1;
      continue;
    }

    if (size == 1 && bytes[0] == 0xE0) {
      bytes[1] = b;
      size = 2;
    }
    else {
      bytes[0] = b;
      size = 1;
    }

    bool make = !(bytes[size - 1] & BIT(7));
    if (kbd_print_scancode(make, size, bytes) != 0) {
      return 1;
    }

    if (size == 1 && bytes[0] == KBD_ESC_BREAK) break;
    size = 0;
  }

  uint8_t cmd_byte;
  if (kbc_read_cmd_byte(&cmd_byte) != 0) return 1;
  if (kbc_write_cmd_byte(cmd_byte | KBC_CB_INT) != 0) return 1;
  return kbd_print_no_sysinb(util_sys_inb_counter);
}

int(kbd_test_timed_scan)(uint8_t n) {
  if (timer_counter == UINT32_MAX) {
    timer_counter = 0;
  }

  uint8_t kbd_bit_no;
  if (kbd_subscribe_int(&kbd_bit_no) != 0) return 1;
  uint32_t irq_set_kbd = BIT(kbd_bit_no);

  uint8_t timer_bit_no;
  if (timer_subscribe_int(&timer_bit_no) != 0) {
    (void) kbd_unsubscribe_int();
    return 1;
  }
  uint32_t irq_set_timer = BIT(timer_bit_no);

  uint32_t idle_ticks = 0;
  uint32_t ticks_per_sec = (uint32_t) sys_hz();

  uint8_t bytes[2];
  uint8_t size = 0;

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
            if (n > 0 && idle_ticks >= (uint32_t) n * ticks_per_sec) {
              goto done;
            }
          }

          if (msg.m_notify.interrupts & irq_set_kbd) {
            kbc_ih();

            if (!kbc_has_byte()) break;
            uint8_t b = kbc_get_byte();
            idle_ticks = 0;

            if (size == 0 && b == 0xE0) {
              bytes[0] = b;
              size = 1;
              break;
            }

            if (size == 1 && bytes[0] == 0xE0) {
              bytes[1] = b;
              size = 2;
            }
            else {
              bytes[0] = b;
              size = 1;
            }

            bool make = !(bytes[size - 1] & BIT(7));
            if (kbd_print_scancode(make, size, bytes) != 0) {
              goto done;
            }

            if (size == 1 && bytes[0] == KBD_ESC_BREAK) {
              goto done;
            }
            size = 0;
          }
          break;

        default:
          break;
      }
    }
  }

done:
  (void) timer_unsubscribe_int();
  (void) kbd_unsubscribe_int();
  return 0;
}
