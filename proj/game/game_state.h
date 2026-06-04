#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "app.h"
#include <stdint.h>

/**
 * @defgroup game_state Game State
 * StateHandler implementation for APP_STATE_PLAYING.
 *
 * Owns all game-session logic: input routing to the Game simulation,
 * edge-triggered mouse fire, and the PLAYING → SCOREBOARD transition
 * on game over.  Transitions are written directly to ctx->current.
 * @{
 */

/** Reset and initialise the Game for a new session. */
void game_state_on_enter    (AppContext *ctx);

/**
 * Advance the game simulation one tick.
 * Sets ctx->current = APP_STATE_SCOREBOARD when the game ends.
 */
void game_state_update      (AppContext *ctx);

/** Clear back-buffer, draw game + HUD, overlay crosshair cursor. */
void game_state_draw        (const AppContext *ctx);

/**
 * Route keyboard scancode to player movement / firing.
 * Sets ctx->current = APP_STATE_EXIT on ESC.
 */
void game_state_handle_key  (AppContext *ctx, uint8_t sc);

/**
 * Update cursor position; edge-detect LB click and fire player bullet.
 * Never changes ctx->current.
 */
void game_state_handle_mouse(AppContext *ctx, int x, int y, bool lb);

/**@}*/

#endif /* GAME_STATE_H */
