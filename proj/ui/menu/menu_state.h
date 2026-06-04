#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "app.h"
#include <stdint.h>

/**
 * @defgroup menu_state Menu State Machine
 * App state callbacks for the main menu (APP_STATE_MENU and APP_STATE_NAME_INPUT).
 * Routes mouse input to menu, handles menu transitions, and manages player name entry.
 * @{
 */

/**
 * Initializes the menu.
 *
 * @param ctx Pointer to application context
 */
void menu_state_on_enter(AppContext *ctx);

/**
 * Updates the menu and checks for button clicks.
 * Transitions based on selected action:
 * - Play → enters name input state
 * - Scores → displays scoreboard
 * - Quit → exits application
 *
 * @param ctx Pointer to application context
 */
void menu_state_update(AppContext *ctx);

/**
 * Renders the active menu screen.
 *
 * @param ctx Pointer to application context
 */
void menu_state_draw(const AppContext *ctx);

/**
 * Routes keyboard input.
 * ESC key exits to application exit state.
 *
 * @param ctx Pointer to application context
 * @param sc  Keyboard scancode
 */
void menu_state_handle_key(AppContext *ctx, uint8_t sc);

/**
 * Updates cursor position and propagates hover/click to menu.
 * Does not change application state.
 *
 * @param ctx Pointer to application context
 * @param x   Mouse X coordinate
 * @param y   Mouse Y coordinate
 * @param lb  Left button pressed flag
 */
void menu_state_handle_mouse(AppContext *ctx, int x, int y, bool lb);

/** @} */

#endif /* MENU_STATE_H */
