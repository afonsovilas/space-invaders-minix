#ifndef MENU_H
#define MENU_H

#include <stdbool.h>

/**
 * @defgroup menu Menu
 * Mouse-controlled main menu for Star Shooter.
 * @{
 */

/** Actions the menu can produce after user interaction. */
typedef enum {
  MENU_ACTION_NONE,  /**< No action yet (user browsing) */
  MENU_ACTION_PLAY,  /**< User clicked PLAY             */
  MENU_ACTION_QUIT   /**< User clicked QUIT             */
} MenuAction;

/** Internal menu state – updated by menu_update(), read by menu_draw(). */
typedef struct {
  int  mouse_x;       /**< Current cursor X              */
  int  mouse_y;       /**< Current cursor Y              */
  bool play_hover;    /**< Cursor is over PLAY button    */
  bool quit_hover;    /**< Cursor is over QUIT button    */
  bool lb_prev;       /**< Previous left-button state    */
  MenuAction action;  /**< Action produced this frame    */
} Menu;

/** Initialise the menu to its default state. */
void menu_init(Menu *m);

/**
 * Update hover / click state.
 * @param m        Menu context
 * @param cursor_x Current mouse X (absolute, screen coords)
 * @param cursor_y Current mouse Y
 * @param lb       Left mouse button currently pressed
 */
void menu_update(Menu *m, int cursor_x, int cursor_y, bool lb);

/** Return the action produced by the last menu_update() call. */
MenuAction menu_get_action(const Menu *m);

/**
 * Render the complete menu into the back-buffer (does NOT flush).
 * Call graphics_flush() after this to present the frame.
 */
void menu_draw(const Menu *m);

/**@}*/

#endif /* MENU_H */
