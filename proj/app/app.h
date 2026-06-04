#ifndef APP_H
#define APP_H

#include "game.h"
#include "menu.h"
#include "name_input.h"
#include "scoreboard.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup app Application Framework
 * Main application state machine and context.
 * @{
 */

/**
 * @enum AppState
 * Main application state enumeration.
 */
typedef enum {
  APP_STATE_MENU        = 0,  /**< Main menu with Play, Scores, Quit */
  APP_STATE_NAME_INPUT  = 1,  /**< Player name entry screen */
  APP_STATE_PLAYING     = 2,  /**< Active game session */
  APP_STATE_SCOREBOARD  = 3,  /**< High scores display */
  APP_STATE_EXIT        = 4   /**< Sentinel: application termination */
} AppState;

/**
 * @struct AppContext
 * Global application state container.
 * Holds all mutable data shared across state handlers (input, UI widgets, game).
 */
typedef struct {
  AppState   current;                           /**< Current app state */
  int        screen_w, screen_h;                /**< Display resolution */
  int        cursor_x, cursor_y;                /**< Mouse cursor position */
  bool       cursor_lb;                         /**< Current left-button state */
  bool       cursor_lb_prev;                    /**< Previous left-button state (for edge detection) */
  char       player_name[SCOREBOARD_NAME_LEN];  /**< Player name from name input */
  Scoreboard scoreboard;                        /**< Loaded high scores */
  Menu       menu;                              /**< Main menu widget */
  NameInput  name_input;                        /**< Player name input widget */
  Game       game;                              /**< Active game session */
} AppContext;

/**
 * @struct StateHandler
 * Callback interface for app state machine handlers.
 * Each app state implements these five functions.
 */
typedef struct {
  /** Called when entering this state */
  void (*on_enter)(AppContext *ctx);
  /** Called once per frame to update state */
  void (*update)(AppContext *ctx);
  /** Called once per frame to render to back-buffer */
  void (*draw)(const AppContext *ctx);
  /** Called when a keyboard scancode is received */
  void (*handle_key)(AppContext *ctx, uint8_t sc);
  /** Called when mouse state changes */
  void (*handle_mouse)(AppContext *ctx, int x, int y, bool lb);
} StateHandler;

/**
 * State handler dispatch table.
 * Maps AppState values to their corresponding StateHandler implementations.
 * Excludes APP_STATE_EXIT (terminal state).
 */
extern const StateHandler STATES[4];

/** @} */

#endif /* APP_H */
