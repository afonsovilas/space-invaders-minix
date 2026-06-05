#include "scoreboard_state.h"
#include "scoreboard.h"
#include "graphics.h"
#include "keyboard.h"

void scoreboard_state_on_enter(AppContext *ctx) {
  /* Scoreboard data is already up to date in ctx->scoreboard —
     game_state calls scoreboard_add before transitioning here. */
  (void) ctx;
}

void scoreboard_state_update(AppContext *ctx) {
  /* No per-tick logic; the scoreboard screen is purely reactive. */
  (void) ctx;
}

void scoreboard_state_draw(const AppContext *ctx) {
  graphics_clear(0x00u);
  const char *name = ctx->game.player.name[0] != '\0' ? ctx->game.player.name : NULL;
  scoreboard_draw(&ctx->scoreboard, ctx->screen_w, ctx->screen_h, name, ctx->game.player.score);
}

void scoreboard_state_handle_key(AppContext *ctx, uint8_t sc) {
  if (sc == KBD_SPACE_MAKE) { ctx->current = APP_STATE_MENU; return; }
  if (sc == KBD_ESC_BREAK)  { ctx->current = APP_STATE_MENU; return; }
}

void scoreboard_state_handle_mouse(AppContext *ctx, int x, int y, bool lb) {
  (void) ctx;
  (void) x; (void) y; (void) lb;
}
