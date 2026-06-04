#ifndef SCOREBOARD_STATE_H
#define SCOREBOARD_STATE_H

#include "app.h"
#include <stdint.h>

/**
 * @defgroup scoreboard_state Scoreboard State
 * StateHandler implementation for APP_STATE_SCOREBOARD.
 *
 * Purely reactive: no per-tick logic beyond rendering.  The scoreboard
 * data already lives in AppContext and is kept up to date by
 * game_state before the transition.  Transitions are written directly
 * to ctx->current.
 * @{
 */

/** No-op: scoreboard data is already current in ctx->scoreboard. */
void scoreboard_state_on_enter    (AppContext *ctx);

/** No per-tick logic; never changes ctx->current. */
void scoreboard_state_update      (AppContext *ctx);

/** Clear back-buffer and render the scoreboard. */
void scoreboard_state_draw        (const AppContext *ctx);

/**
 * Sets ctx->current on:
 *   SPACE → APP_STATE_MENU
 *   ESC   → APP_STATE_EXIT
 */
void scoreboard_state_handle_key  (AppContext *ctx, uint8_t sc);

/** Ignored in this state; never changes ctx->current. */
void scoreboard_state_handle_mouse(AppContext *ctx, int x, int y, bool lb);

/**@}*/

#endif /* SCOREBOARD_STATE_H */
