#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

static int hook_id = 0;
unsigned int timer_counter = 0;


int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (timer > 2 || freq <= 0) return -1;

  // Calculate divisor to achieve the desired frequency
  uint32_t divisor = TIMER_FREQ / freq;

  if (divisor <= 0 || divisor > 0xFFFF) return -1;

  uint8_t st;

  if (timer_get_conf(timer, &st) != 0)
    return -1;

  // Preserve the 4 least significant bits (mode and BCD)
  uint8_t preserved = st & 0x0F;

  // Build control word: select timer + load LSB then MSB + preserved mode bits
  uint8_t control = (timer << 6) | TIMER_LSB_MSB | preserved;

  // Send control word to the timer control register
  if (sys_outb(TIMER_CTRL, control) != 0) return -1;

  // Split divisor into LSB and MSB
  uint8_t lsb = divisor & 0xFF;
  uint8_t msb = (divisor >> 8) & 0xFF;

  uint16_t timer_port;

  // Select the correct timer port
  switch (timer) {
    case 0: timer_port = TIMER_0; break;
    case 1: timer_port = TIMER_1; break;
    case 2: timer_port = TIMER_2; break;
    default: return -1;
  }

  // Write LSB to timer
  if (sys_outb(timer_port, lsb) != 0) return -1;

  // Write MSB to timer
  if (sys_outb(timer_port, msb) != 0) return -1;

  return 0;
}


int (timer_subscribe_int)(uint8_t *bit_no) {

  if (bit_no == NULL) return -1;

  *bit_no = hook_id;

  // Request subscription to timer interrupts
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK) return -1;

  return 0;
}


int (timer_unsubscribe_int)() {

  // Remove interrupt subscription
  if (sys_irqrmpolicy(&hook_id) != OK) return -1;

  return 0;
}


void (timer_int_handler)() {
  timer_counter++;
}


int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  if (st == NULL) return -1;

  // Build read-back command to request timer status
  uint8_t rb_cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  // Send read-back command to control register
  if (sys_outb(TIMER_CTRL, rb_cmd) != 0) return -1;

  int timer_port;

  // Select the correct timer port
  switch (timer) {
    case 0: timer_port = TIMER_0; break;
    case 1: timer_port = TIMER_1; break;
    case 2: timer_port = TIMER_2; break;
  }

  // Read the status byte from the timer
  if (util_sys_inb(timer_port, st) != 0) return -1;

  return 0;
}


int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  union timer_status_field_val data;

  switch (field) {

  case tsf_all:
    // Return the full status byte
    data.byte = st;
    break;

  case tsf_initial:
    // Extract initialization mode (bits 4 and 5)
    data.in_mode = (st & TIMER_LSB_MSB) >> 4;
    break;

  case tsf_mode:
    // Extract counting mode (bits 1 to 3)
    data.count_mode = (st >> 1) & 0x07;

    // Normalize modes 6 and 7 to 2 and 3 (hardware aliasing)
    if (data.count_mode == 6) data.count_mode = 2;
    if (data.count_mode == 7) data.count_mode = 3;
    break;

  case tsf_base:
    // Extract BCD/binary mode (bit 0)
    data.bcd = st & TIMER_BCD;
    break;

  default:
    return -1;
  }

  // Print the timer configuration
  if (timer_print_config(timer, field, data) != 0) return -1;

  return 0;
}
