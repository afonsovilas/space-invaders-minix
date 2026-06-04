#ifndef NAME_INPUT_STATE_H
#define NAME_INPUT_STATE_H

#include "app.h"
#include <stdint.h>

/**
 * @defgroup name_input_state Name Input State Machine
 * App state callbacks for the name input screen.
 * Handles keyboard input, updates the name widget, and manages state transitions.
 * @{
 */

/**
 * Initializes the name input state when entered.
 *
 * @param ctx Pointer to the application context
 */
void name_input_state_on_enter(AppContext *ctx);

/**
 * Updates the name input state (animation and cursor blinking).
 * Called once per frame while in the name input state.
 *
 * @param ctx Pointer to the application context
 */
void name_input_state_update(AppContext *ctx);

/**
 * Renders the name input screen.
 *
 * @param ctx Pointer to the application context
 */
void name_input_state_draw(const AppContext *ctx);

/**
 * Handles keyboard input during name input state.
 * Accepts letter keys, backspace, Enter (to confirm), and Esc (to cancel).
 * Updates the name input widget and transitions app state on confirmation or cancellation.
 *
 * @param ctx Pointer to the application context
 * @param sc  Keyboard scancode to process
 */
void name_input_state_handle_key(AppContext *ctx, uint8_t sc);

/**
 * Handles mouse input during name input state.
 * Currently unused (name input is keyboard-only).
 *
 * @param ctx Pointer to the application context
 * @param x   Mouse X coordinate
 * @param y   Mouse Y coordinate
 * @param lb  Left button pressed flag
 */
void name_input_state_handle_mouse(AppContext *ctx, int x, int y, bool lb);

/** @} */

#endif /* NAME_INPUT_STATE_H */
