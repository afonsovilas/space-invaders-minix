#ifndef MOUSE_H
#define MOUSE_H

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup mouse PS/2 Mouse Driver
 * Interrupt-driven PS/2 mouse driver in stream mode.
 * Provides cursor position tracking and button state.
 * @{
 */

/**
 * @struct MouseState
 * Current mouse state: position and button flags.
 * Updated when a complete 3-byte packet is received.
 */
typedef struct {
  int  x;        /**< Absolute cursor X (screen pixels, clamped) */
  int  y;        /**< Absolute cursor Y (screen pixels, clamped) */
  bool lb;       /**< Left button pressed */
  bool rb;       /**< Right button pressed */
  bool mb;       /**< Middle button pressed */
  bool lb_prev;  /**< Previous left button state (for edge detection) */
} MouseState;

/**
 * Subscribes to mouse IRQ12.
 * Must be called before the mouse can generate interrupts.
 *
 * @param bit_no Pointer where the IRQ bit number will be stored
 * @return 0 on success, non-zero on failure
 */
int (mouse_subscribe_int)(uint8_t *bit_no);

/**
 * Unsubscribes from mouse IRQ12.
 *
 * @return 0 on success, non-zero on failure
 */
int (mouse_unsubscribe_int)(void);

/**
 * Mouse interrupt handler.
 * Must be called by the main interrupt dispatcher on mouse IRQ.
 */
void (mouse_ih)(void);

/**
 * Sends a command byte to the mouse and waits for ACK.
 *
 * @param cmd Command byte to send
 * @return 0 on success, non-zero on failure
 */
int proj_mouse_send_cmd(uint8_t cmd);

/**
 * Disables mouse data reporting (sends 0xF5).
 *
 * @return 0 on success, non-zero on failure
 */
int proj_mouse_disable_reporting(void);

/**
 * Enables mouse data reporting in stream mode (sends 0xF4).
 *
 * @return 0 on success, non-zero on failure
 */
int proj_mouse_enable_reporting(void);

/**
 * Sets mouse to stream mode (sends 0xEA).
 *
 * @return 0 on success, non-zero on failure
 */
int proj_mouse_set_stream_mode(void);

/**
 * Checks if a new mouse data byte is available.
 *
 * @return true if data available, false otherwise
 */
bool mouse_has_byte(void);

/**
 * Retrieves and clears the last mouse data byte.
 *
 * @return The last byte read by the interrupt handler
 */
uint8_t mouse_get_byte(void);

/**
 * Performs complete mouse initialization sequence.
 * Disables reporting, enables AUX, sets stream mode, enables reporting.
 *
 * @return 0 on success, non-zero on failure
 */
int mouse_setup_stream(void);

/**
 * Checks if the given byte is a valid first byte of a mouse packet.
 *
 * @param byte The byte to check.
 * @return true if the byte has the synchronization bit set.
 */
bool mouse_packet_is_sync(uint8_t byte);

/**
 * Checks if a complete 3-byte mouse packet has an X or Y overflow.
 *
 * @param packet The 3-byte packet to check.
 * @return true if there is an overflow, false otherwise.
 */
bool mouse_packet_is_overflow(const uint8_t packet[3]);

/**
 * Parses a valid 3-byte mouse packet, extracting deltas and the left button state.
 *
 * @param packet The 3-byte packet to parse.
 * @param dx Pointer to store the X movement.
 * @param dy Pointer to store the Y movement.
 * @param lb Pointer to store the left button state.
 */
void mouse_packet_parse(const uint8_t packet[3], int16_t *dx, int16_t *dy, bool *lb);

/** @} */

#endif /* MOUSE_H */
