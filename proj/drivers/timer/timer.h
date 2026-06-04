#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/**
 * @defgroup timer Timer Driver
 * Game timer driver using the system timer for frame pacing and timing.
 * @{
 */

/**
 * Subscribes to the timer interrupt.
 * Must be called before the timer can generate interrupts.
 *
 * @param bit_no Pointer where the IRQ bit number will be stored
 * @return 0 on success, non-zero on failure
 */
int game_timer_subscribe_int(uint8_t *bit_no);

/**
 * Unsubscribes from the timer interrupt.
 *
 * @return 0 on success, non-zero on failure
 */
int game_timer_unsubscribe_int(void);

/**
 * Timer interrupt handler.
 * Must be called by the main interrupt dispatcher on each timer interrupt.
 */
void game_timer_ih(void);

/**
 * Retrieves the current timer counter value.
 *
 * @return The current timer tick count
 */
uint32_t game_timer_get_counter(void);

/** @} */

#endif