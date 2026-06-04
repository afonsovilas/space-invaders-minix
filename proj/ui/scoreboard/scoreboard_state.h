#ifndef SCOREBOARD_STATE_H
#define SCOREBOARD_STATE_H

#include "app.h"
#include <stdint.h>

/**
 * @defgroup scoreboard_state Scoreboard State Machine
 * App state callbacks for displaying high scores (APP_STATE_SCOREBOARD).
 * Purely reactive; scoreboard data is already loaded in the app context.
 * @{
 */

/**
 * Initializes the scoreboard state.
 * Scoreboard data is already current in the application context.
 *
 * @param ctx Pointer to application context
 */
void scoreboard_state_on_enter(AppContext *ctx);

/**
 * Updates the scoreboard state.
 * No per-tick logic; does not change application state.
 *
 * @param ctx Pointer to application context
 */
void scoreboard_state_update(AppContext *ctx);

/**
 * Renders the scoreboard display.
 *
 * @param ctx Pointer to application context
 */
void scoreboard_state_draw(const AppContext *ctx);

/**
 * Routes keyboard input.
 * - Space → returns to main menu
 * - ESC → exits application
 *
 * @param ctx Pointer to application context
 * @param sc  Keyboard scancode
 */
void scoreboard_state_handle_key(AppContext *ctx, uint8_t sc);

/**
 * Handles mouse input.
 * Ignored in this state; does not change application state.
 *
 * @param ctx Pointer to application context
 * @param x   Mouse X coordinate
 * @param y   Mouse Y coordinate
 * @param lb  Left button pressed flag
 */
void scoreboard_state_handle_mouse(AppContext *ctx, int x, int y, bool lb);

/** @} */

#endif /* SCOREBOARD_STATE_H */
