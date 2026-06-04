#ifndef APP_H
#define APP_H

#include "game.h"
#include "menu.h"
#include "name_input.h"
#include "scoreboard.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  APP_STATE_MENU        = 0,
  APP_STATE_NAME_INPUT  = 1,
  APP_STATE_PLAYING     = 2,
  APP_STATE_SCOREBOARD  = 3,
  APP_STATE_EXIT        = 4   /* sentinel – stops the loop */
} AppState;

typedef struct {
  AppState current;
  int screen_w;
  int screen_h;
  int  cursor_x;
  int  cursor_y;
  bool cursor_lb;
  bool cursor_lb_prev;
  char       player_name[SCOREBOARD_NAME_LEN];
  Scoreboard scoreboard;
  Menu       menu;
  NameInput  name_input;
  Game       game;
} AppContext;

typedef struct {
  void (*on_enter)    (AppContext *ctx);
  void (*update)      (AppContext *ctx);
  void (*draw)        (const AppContext *ctx);
  void (*handle_key)  (AppContext *ctx, uint8_t sc);
  void (*handle_mouse)(AppContext *ctx, int x, int y, bool lb);
} StateHandler;

extern const StateHandler STATES[4];

#endif /* APP_H */
