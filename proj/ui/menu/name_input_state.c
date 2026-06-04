#include <string.h>

#include "name_input_state.h"
#include "name_input.h"
#include "graphics.h"
#include "scoreboard.h"

void name_input_state_on_enter(AppContext *ctx) {
  name_input_init(&ctx->name_input);
}

void name_input_state_update(AppContext *ctx) {
  name_input_update(&ctx->name_input);
}

void name_input_state_draw(const AppContext *ctx) {
  graphics_clear(0x00u);
  name_input_draw(&ctx->name_input);
}

void name_input_state_handle_key(AppContext *ctx, uint8_t sc) {
  NameInputResult result = name_input_handle_key(&ctx->name_input, sc);
  if (result == NAME_INPUT_CONFIRMED) {
    strncpy(ctx->player_name, ctx->name_input.name, SCOREBOARD_NAME_LEN - 1);
    ctx->player_name[SCOREBOARD_NAME_LEN - 1] = '\0';
    ctx->current = APP_STATE_PLAYING;
  } else if (result == NAME_INPUT_CANCELLED) {
    ctx->current = APP_STATE_MENU;
  }
}

void name_input_state_handle_mouse(AppContext *ctx, int x, int y, bool lb) {
  (void) ctx;
  (void) x; (void) y; (void) lb;
}
