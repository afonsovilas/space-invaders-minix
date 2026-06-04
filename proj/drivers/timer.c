#include <lcom/lcf.h>

#include <stdint.h>

#include "timer.h"

#define TIMER_IRQ 0

static int timer_hook_id = 0;
static uint32_t timer_counter = 0;

int game_timer_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;

  *bit_no = (uint8_t) timer_hook_id;

  if (sys_irqsetpolicy(TIMER_IRQ, IRQ_REENABLE, &timer_hook_id) != 0) {
    return 1;
  }

  return 0;
}

int game_timer_unsubscribe_int(void) {
  if (sys_irqrmpolicy(&timer_hook_id) != 0) {
    return 1;
  }

  return 0;
}

void game_timer_ih(void) {
  timer_counter++;
}

uint32_t game_timer_get_counter(void) {
  return timer_counter;
}