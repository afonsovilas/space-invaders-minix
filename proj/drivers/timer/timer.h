#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

int game_timer_subscribe_int(uint8_t *bit_no);
int game_timer_unsubscribe_int(void);
void game_timer_ih(void);
uint32_t game_timer_get_counter(void);

#endif