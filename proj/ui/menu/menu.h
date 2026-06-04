#ifndef MENU_H
#define MENU_H

#include "scoreboard.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup menu Menu
 * Mouse-controlled main menu and keyboard-driven name-entry screen
 * for Star Shooter.
 * @{
 */

/** Which screen the menu module is currently showing. */
typedef enum {
  MENU_SCREEN_MAIN,       /**< PLAY / SCORES / QUIT buttons  */
  MENU_SCREEN_NAME_INPUT  /**< Keyboard name-entry screen    */
} MenuScreen;

/** Actions the menu can produce after user interaction. */
typedef enum {
  MENU_ACTION_NONE,    /**< No action yet                 */
  MENU_ACTION_PLAY,    /**< Name confirmed — start game   */
  MENU_ACTION_SCORES,  /**< User clicked SCORES           */
  MENU_ACTION_QUIT     /**< User clicked QUIT             */
} MenuAction;

/** Internal menu state – updated by menu_update(), read by menu_draw(). */
typedef struct {
  /* ---- Main screen ---- */
  MenuScreen screen;        /**< Active sub-screen             */
  int  mouse_x;             /**< Current cursor X              */
  int  mouse_y;             /**< Current cursor Y              */
  bool play_hover;          /**< Cursor is over PLAY button    */
  bool scores_hover;        /**< Cursor is over SCORES button  */
  bool quit_hover;          /**< Cursor is over QUIT button    */
  bool lb_prev;             /**< Previous left-button state    */
  MenuAction action;        /**< Action produced this frame    */

  /* ---- Name input screen ---- */
  char player_name[SCOREBOARD_NAME_LEN]; /**< Typed name buffer          */
  int  name_len;            /**< Current number of chars typed */
  bool name_blink_show;     /**< Cursor block visible          */
  int  name_blink_timer;    /**< Countdown to next blink       */
  bool name_empty_warning;  /**< Show "NAME CANNOT BE EMPTY"   */
} Menu;

/** Initialise (or reset) the menu to its default state. */
void menu_init(Menu *m);

/**
 * Update hover / click state (main screen only).
 * @param m        Menu context
 * @param cursor_x Current mouse X (absolute, screen coords)
 * @param cursor_y Current mouse Y
 * @param lb       Left mouse button currently pressed
 */
void menu_update(Menu *m, int cursor_x, int cursor_y, bool lb);

/** Return the action produced by the last interaction. */
MenuAction menu_get_action(const Menu *m);

/**
 * Render the active screen into the back-buffer (does NOT flush).
 * Dispatches to the main buttons screen or the name-entry screen
 * based on menu->screen.
 */
void menu_draw(const Menu *m);

/**
 * Process a keyboard scancode while the name-entry screen is active.
 * – Letter keys       → append uppercase char (up to SCOREBOARD_NAME_LEN-1)
 * – KBD_BACKSPACE_MAKE → delete last char
 * – KBD_ENTER_MAKE    → confirm (sets MENU_ACTION_PLAY) or warn if empty
 * – KBD_ESC_BREAK     → cancel, return to main buttons screen
 */
void menu_name_input_handle_key(Menu *m, uint8_t scancode);

/** Advance the cursor blink timer. Call once per timer tick. No-op on main screen. */
void menu_name_input_update(Menu *m);

/** Returns true while the name-entry screen is active. */
bool menu_is_name_input_active(const Menu *m);

/**
 * Returns the confirmed player name.
 * Only valid after menu_get_action() == MENU_ACTION_PLAY.
 */
const char *menu_get_player_name(const Menu *m);

/**@}*/

#endif /* MENU_H */
