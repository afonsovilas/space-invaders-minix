#include <string.h>

#include "name_input_state.h"
#include "name_input.h"
#include "graphics.h"
#include "scoreboard.h"
#include "keyboard.h"

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
  NameInput *ni = &ctx->name_input;

  if (sc == KBD_ESC_BREAK) {
    ctx->current = APP_STATE_MENU;
    return;
  }

  if (sc == KBD_ENTER_MAKE) {
    if (ni->name_len > 0) {
      ni->name[ni->name_len] = '\0';
      strncpy(ctx->player_name, ni->name, SCOREBOARD_NAME_LEN - 1);
      ctx->player_name[SCOREBOARD_NAME_LEN - 1] = '\0';
      ctx->current = APP_STATE_PLAYING;
    } else {
      ni->empty_warning = true;
    }
    return;
  }

  if (sc == KBD_BACKSPACE_MAKE) {
    if (ni->name_len > 0) {
      ni->name_len--;
      ni->name[ni->name_len] = '\0';
      ni->empty_warning = false;
    }
    return;
  }

  if (sc < 128) {
    char ch = kbd_scancode_to_char[sc];
    if (ch != '\0' && ni->name_len < SCOREBOARD_NAME_LEN - 1) {
      ni->name[ni->name_len++] = ch;
      ni->name[ni->name_len]   = '\0';
      ni->empty_warning = false;
    }
  }
}

void name_input_state_handle_mouse(AppContext *ctx, int x, int y, bool lb) {
  (void) ctx;
  (void) x; (void) y; (void) lb;
}
