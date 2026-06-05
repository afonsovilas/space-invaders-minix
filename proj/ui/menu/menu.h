#ifndef MENU_H
#define MENU_H

#include "scoreboard.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup menu Main Menu Widget
 * Interactive menu screen with mouse-driven button selection for Play, Scores, and Quit.
 * @{
 */

/**
 * @enum MenuAction
 * Action triggered by menu interaction.
 */
typedef enum {
  MENU_ACTION_NONE,    /**< No action selected */
  MENU_ACTION_PLAY,    /**< Play button clicked */
  MENU_ACTION_SCORES,  /**< View high scores button clicked */
  MENU_ACTION_QUIT     /**< Quit button clicked */
} MenuAction;

/**
 * @struct Menu
 * Tracks menu state: cursor position, button hover states, and pending action.
 */
typedef struct {
  int  mouse_x, mouse_y;   /**< Current mouse cursor position */
  bool play_hover;         /**< Play button is hovered */
  bool scores_hover;       /**< Scores button is hovered */
  bool quit_hover;         /**< Quit button is hovered */
  bool lb_prev;            /**< Previous frame left-button state */
  MenuAction action;       /**< Pending menu action from click */
} Menu;

/**
 * Initializes the Menu widget.
 *
 * @param m Pointer to the Menu structure to initialize
 */
void menu_init(Menu *m);

/**
 * Updates the Menu widget with mouse input and click detection.
 * Computes hover states and generates MenuAction when buttons are clicked.
 *
 * @param m        Pointer to the Menu structure to update
 * @param cursor_x Mouse X coordinate
 * @param cursor_y Mouse Y coordinate
 * @param lb       Left mouse button pressed flag
 */
void menu_update(Menu *m, int cursor_x, int cursor_y, bool lb);

/**
 * Retrieves the pending menu action.
 *
 * @param m Pointer to the Menu structure
 * @return The current MenuAction (MENU_ACTION_NONE if nothing clicked)
 */
MenuAction menu_get_action(const Menu *m);

/**
 * Renders the Menu widget to the back-buffer.
 * Draws title, buttons with hover effects, and mouse cursor.
 *
 * @param m Pointer to the Menu structure to render
 */
void menu_draw(const Menu *m);

/** @} */

#endif /* MENU_H */
