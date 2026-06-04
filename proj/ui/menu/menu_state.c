#include "menu_state.h"
#include "menu.h"
#include "keyboard.h"

void menu_state_on_enter(AppContext *ctx) {
  menu_init(&ctx->menu);
}

void menu_state_update(AppContext *ctx) {
  MenuAction action = menu_get_action(&ctx->menu);
  if (action == MENU_ACTION_PLAY)   { ctx->current = APP_STATE_NAME_INPUT; return; }
  if (action == MENU_ACTION_SCORES) { ctx->current = APP_STATE_SCOREBOARD; return; }
  if (action == MENU_ACTION_QUIT)   { ctx->current = APP_STATE_EXIT;        return; }
}

void menu_state_draw(const AppContext *ctx) {
  menu_draw(&ctx->menu);
}

void menu_state_handle_key(AppContext *ctx, uint8_t sc) {
  if (sc == KBD_ESC_BREAK) ctx->current = APP_STATE_EXIT;
}

void menu_state_handle_mouse(AppContext *ctx, int x, int y, bool lb) {
  ctx->cursor_x = x;
  ctx->cursor_y = y;
  ctx->cursor_lb = lb;
  menu_update(&ctx->menu, x, y, lb);
}
