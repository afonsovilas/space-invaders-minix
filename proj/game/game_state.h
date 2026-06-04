#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "app.h"
#include <stdint.h>

/**
 * @defgroup game_state Game State Machine
 * App state callbacks for active gameplay (APP_STATE_PLAYING).
 * Routes input to game, detects game over, and transitions to scoreboard.
 * @{
 */

/**
 * Initializes the game for a new session.
 *
 * @param ctx Pointer to application context
 */
void game_state_on_enter(AppContext *ctx);

/**
 * Advances game simulation one frame.
 * Transitions to scoreboard when game ends.
 *
 * @param ctx Pointer to application context
 */
void game_state_update(AppContext *ctx);

/**
 * Renders the game and HUD, overlays the mouse cursor.
 *
 * @param ctx Pointer to application context
 */
void game_state_draw(const AppContext *ctx);

/**
 * Routes keyboard input to player movement and firing.
 * ESC key exits to main menu.
 *
 * @param ctx Pointer to application context
 * @param sc  Keyboard scancode
 */
void game_state_handle_key(AppContext *ctx, uint8_t sc);

/**
 * Updates cursor position and detects mouse clicks to fire bullets.
 * Does not change application state.
 *
 * @param ctx Pointer to application context
 * @param x   Mouse X coordinate
 * @param y   Mouse Y coordinate
 * @param lb  Left button pressed flag
 */
void game_state_handle_mouse(AppContext *ctx, int x, int y, bool lb);

/** @} */

#endif /* GAME_STATE_H */
