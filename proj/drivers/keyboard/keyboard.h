#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup keyboard Keyboard Driver
 * Low-level keyboard driver for interrupt-driven scancode capture and conversion.
 * @{
 */

/**
 * @name Special Key Scancodes
 * @{
 */
#define KBD_ESC_BREAK       0x81  /**< Escape key release */
#define KBD_BACKSPACE_MAKE  0x0E  /**< Backspace key press */
#define KBD_ENTER_MAKE      0x1C  /**< Enter key press */
#define KBD_SPACE_MAKE      0x39  /**< Space key press */
#define KBD_SPACE_BREAK     0xB9  /**< Space key release */
/** @} */

/**
 * @name Letter Key Scancodes (Top Row)
 * @{
 */
#define KBD_Q_MAKE  0x10
#define KBD_W_MAKE  0x11
#define KBD_E_MAKE  0x12
#define KBD_R_MAKE  0x13
#define KBD_T_MAKE  0x14
#define KBD_Y_MAKE  0x15
#define KBD_U_MAKE  0x16
#define KBD_I_MAKE  0x17
#define KBD_O_MAKE  0x18
#define KBD_P_MAKE  0x19
/** @} */

/**
 * @name Letter Key Scancodes (Home Row)
 * @{
 */
#define KBD_A_MAKE  0x1E
#define KBD_A_BREAK 0x9E
#define KBD_S_MAKE  0x1F
#define KBD_D_MAKE  0x20
#define KBD_D_BREAK 0xA0
#define KBD_F_MAKE  0x21
#define KBD_G_MAKE  0x22
#define KBD_H_MAKE  0x23
#define KBD_J_MAKE  0x24
#define KBD_K_MAKE  0x25
#define KBD_L_MAKE  0x26
/** @} */

/**
 * @name Letter Key Scancodes (Bottom Row)
 * @{
 */
#define KBD_Z_MAKE  0x2C
#define KBD_X_MAKE  0x2D
#define KBD_C_MAKE  0x2E
#define KBD_V_MAKE  0x2F
#define KBD_B_MAKE  0x30
#define KBD_N_MAKE  0x31
#define KBD_M_MAKE  0x32
/** @} */

/**
 * Conversion table: scancode (make-code 0x00–0x7F) to uppercase ASCII letter.
 * Characters are '\0' if the key does not produce a letter.
 */
extern const char kbd_scancode_to_char[128];

/**
 * Subscribes to keyboard interrupt.
 * Must be called before any keyboard input can be received.
 *
 * @param bit_no Pointer where the IRQ bit number will be stored
 * @return 0 on success, non-zero on failure
 */
int keyboard_subscribe_int(uint8_t *bit_no);

/**
 * Unsubscribes from keyboard interrupt.
 * Call this when the keyboard driver is no longer needed.
 *
 * @return 0 on success, non-zero on failure
 */
int keyboard_unsubscribe_int(void);

/**
 * Keyboard interrupt handler.
 * Must be called by the main interrupt dispatcher when a keyboard interrupt occurs.
 */
void keyboard_ih(void);

/**
 * Checks if keyboard data is available.
 *
 * @return true if a scancode has been received and not yet read, false otherwise
 */
bool keyboard_has_data(void);

/**
 * Retrieves the last keyboard scancode.
 * Must only be called after keyboard_has_data() returns true.
 *
 * @return The keyboard scancode (8-bit value, 0x00–0xFF)
 */
uint8_t keyboard_get_byte(void);

/**
 * Clears the keyboard data buffer after reading.
 * Call this after processing a scancode.
 */
void keyboard_clear_data(void);

/** @} */

#endif