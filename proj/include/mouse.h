#ifndef MOUSE_H
#define MOUSE_H

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup mouse Mouse
 * PS/2 mouse driver – interrupt-driven, stream mode.
 * Pattern mirrors lab4/mouse.c.
 *
 * All function declarations that are wrapped by LCF use the parenthesised form
 *   int (func_name)(...)
 * to suppress macro expansion (same convention as lab4.h).
 * @{
 */

/**
 * Tracks the absolute screen-space cursor position and button state.
 * Updated every time a full 3-byte packet is processed.
 */
typedef struct {
  int  x;        /**< Absolute cursor X in pixels (clamped to screen) */
  int  y;        /**< Absolute cursor Y in pixels (clamped to screen) */
  bool lb;       /**< Left  button currently pressed */
  bool rb;       /**< Right button currently pressed */
  bool mb;       /**< Middle button currently pressed */
  bool lb_prev;  /**< Previous-tick left button state (edge detection) */
} MouseState;

/*
 * Declarations below mirror lab4.h.
 * Parentheses around each name suppress the LCF macro expansion so this
 * header can be included before or after <lcom/lcf.h> without conflict.
 */

/** Subscribe to mouse IRQ12; stores the bit position in *bit_no. */
int (mouse_subscribe_int)(uint8_t *bit_no);

/** Remove the mouse IRQ subscription. */
int (mouse_unsubscribe_int)(void);

/** Mouse interrupt handler – reads one byte from the KBC output buffer. */
void (mouse_ih)(void);

/** Send a single-byte command to the mouse and wait for ACK. */
int proj_mouse_send_cmd(uint8_t cmd);

/** Disable mouse data reporting (send 0xF5). */
int proj_mouse_disable_reporting(void);

/** Enable mouse data reporting in stream mode (send 0xF4). */
int proj_mouse_enable_reporting(void);

/** Set the mouse to stream mode (send 0xEA). */
int proj_mouse_set_stream_mode(void);

/*
 * Project-specific helpers – NOT present in lab4.h, so no parentheses needed.
 */

/** Returns true if a new byte is available from the last interrupt. */
bool mouse_has_byte(void);

/** Returns and clears the last byte read by the interrupt handler. */
uint8_t mouse_get_byte(void);

/**
 * Full mouse initialisation sequence (mirrors lab4's mouse_setup_stream):
 *   disable reporting → enable AUX → enable mouse IRQ in KBC → stream mode → enable reporting.
 */
int mouse_setup_stream(void);

/**@}*/

#endif /* MOUSE_H */
