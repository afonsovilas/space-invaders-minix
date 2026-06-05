#include "game_state.h"
#include "game.h"
#include "graphics.h"
#include "keyboard.h"
#include "scoreboard.h"
#include "background.h"

#include <string.h>

void game_state_on_enter(AppContext *ctx) {
  game_init(&ctx->game, ctx->screen_w, ctx->screen_h);
  strncpy(ctx->game.player.name, ctx->player_name, SCOREBOARD_NAME_LEN - 1);
  ctx->game.player.name[SCOREBOARD_NAME_LEN - 1] = '\0';
  /* Reset click edge-detection state for the new session */
  ctx->cursor_lb_prev = false;
}

void game_state_update(AppContext *ctx) {
  if (!game_update(&ctx->game)) {
    scoreboard_add(&ctx->scoreboard, ctx->game.player.name, ctx->game.player.score);
    ctx->current = APP_STATE_SCOREBOARD;
  }
}

void game_state_draw(const AppContext *ctx) {
  draw_game_background();
  game_draw(&ctx->game);
}

void game_state_handle_key(AppContext *ctx, uint8_t sc) {
  if (sc == KBD_ESC_BREAK)  { ctx->current = APP_STATE_MENU; return; }

  if (sc == KBD_A_MAKE)     ctx->game.player.moving_left  = true;
  if (sc == KBD_A_BREAK)    ctx->game.player.moving_left  = false;
  if (sc == KBD_D_MAKE)     ctx->game.player.moving_right = true;
  if (sc == KBD_D_BREAK)    ctx->game.player.moving_right = false;
  if (sc == KBD_SPACE_MAKE) game_fire_player_bullet(&ctx->game);
}

void game_state_handle_mouse(AppContext *ctx, int x, int y, bool lb) {
  ctx->cursor_x  = x;
  ctx->cursor_y  = y;
  ctx->cursor_lb = lb;

  /* Edge-detect left-click: fire on the rising edge of the button press */
  if (lb && !ctx->cursor_lb_prev) {
    game_fire_player_bullet(&ctx->game);
  }
  ctx->cursor_lb_prev = lb;
}
