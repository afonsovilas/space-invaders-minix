#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "app.h"
#include <stdint.h>

/**
 * @defgroup menu_state Menu State
 * StateHandler implementation for APP_STATE_MENU.
 *
 * Bridges the AppContext to the Menu module.  All menu logic stays in
 * menu.c / menu.h; this layer only translates MenuAction values into
 * ctx->current transitions and copies shared data (player name) at the
 * right moment.
 * @{
 */

/** Reset the Menu to its initial state. */
void menu_state_on_enter    (AppContext *ctx);

/**
 * Tick the blink timer and check for a pending MenuAction.
 * Sets ctx->current on transition:
 *   MENU_ACTION_PLAY   → APP_STATE_PLAYING  (copies player name first)
 *   MENU_ACTION_SCORES → APP_STATE_SCOREBOARD
 *   MENU_ACTION_QUIT   → APP_STATE_EXIT
 */
void menu_state_update      (AppContext *ctx);

/** Render the active menu screen into the back-buffer. */
void menu_state_draw        (const AppContext *ctx);

/**
 * Dispatch keyboard input.
 * While name-entry is active: delegates entirely to menu_name_input_handle_key.
 * On main screen: sets ctx->current = APP_STATE_EXIT on ESC.
 */
void menu_state_handle_key  (AppContext *ctx, uint8_t sc);

/**
 * Update cursor position and propagate hover / click to Menu.
 * Never changes ctx->current.
 */
void menu_state_handle_mouse(AppContext *ctx, int x, int y, bool lb);

/**@}*/

#endif /* MENU_STATE_H */
