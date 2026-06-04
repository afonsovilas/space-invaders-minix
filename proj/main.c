#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "app.h"
#include "graphics.h"
#include "keyboard.h"
#include "timer.h"
#include "mouse.h"
#include "scoreboard.h"
#include "utils.h"
#include "i8042.h"

/* ------------------------------------------------------------------ */
/* Helper: apply a state transition if ctx->current changed           */
/* ------------------------------------------------------------------ */

/**
 * Called after every handler invocation.
 * If ctx->current changed from `before`, updates the active handler
 * pointer and calls on_enter for the new state.
 * Returns false if the loop should stop (APP_STATE_EXIT).
 */
static bool apply_transition(AppContext *ctx,
                              AppState before,
                              const StateHandler **state) {
  if (ctx->current == APP_STATE_EXIT) return false;
  if (ctx->current != before) {
    *state = &STATES[ctx->current];
    (*state)->on_enter(ctx);
  }
  return true;
}

/* ------------------------------------------------------------------ */
/* LCF entry points                                                    */
/* ------------------------------------------------------------------ */

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/grupo_2leic02_1/proj/trace.txt");
  lcf_log_output("/home/lcom/labs/grupo_2leic02_1/proj/output.txt");
  if (lcf_start(argc, argv)) return 1;
  lcf_cleanup();
  return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {
  (void) argc; (void) argv;

  /* ---- Graphics ---- */
  if (graphics_init(0x105) != 0) {
    printf("graphics_init failed\n");
    return 1;
  }

  /* ---- Application context ---- */
  AppContext ctx;
  memset(&ctx, 0, sizeof(ctx));
  ctx.current  = APP_STATE_MENU;
  ctx.screen_w = (int) graphics_get_hres();
  ctx.screen_h = (int) graphics_get_vres();
  ctx.cursor_x = ctx.screen_w / 2;
  ctx.cursor_y = ctx.screen_h / 2;

  /* Load persistent scoreboard once at startup */
  scoreboard_load(&ctx.scoreboard);

  /* Pre-init game entity storage (game_state_on_enter re-inits on each play) */
  if (game_init(&ctx.game, ctx.screen_w, ctx.screen_h) != 0) {
    printf("game_init failed\n");
    vg_exit();
    return 1;
  }

  /* ---- Subscribe interrupts ---- */
  uint8_t kbd_bit, timer_bit, mouse_bit;

  if (keyboard_subscribe_int(&kbd_bit) != 0) {
    printf("keyboard_subscribe_int failed\n");
    vg_exit(); return 1;
  }
  if (game_timer_subscribe_int(&timer_bit) != 0) {
    printf("game_timer_subscribe_int failed\n");
    keyboard_unsubscribe_int(); vg_exit(); return 1;
  }
  if (mouse_subscribe_int(&mouse_bit) != 0) {
    printf("mouse_subscribe_int failed\n");
    game_timer_unsubscribe_int(); keyboard_unsubscribe_int(); vg_exit(); return 1;
  }
  if (mouse_setup_stream() != 0)
    printf("mouse_setup_stream failed (continuing)\n");

  const uint32_t kbd_irq   = BIT(kbd_bit);
  const uint32_t timer_irq = BIT(timer_bit);
  const uint32_t mouse_irq = BIT(mouse_bit);

  /* ---- Mouse packet accumulator ---- */
  uint8_t mouse_bytes[3];
  uint8_t mouse_idx = 0;

  /* ---- Enter initial state ---- */
  const StateHandler *state = &STATES[APP_STATE_MENU];
  state->on_enter(&ctx);

  /* Initial draw */
  state->draw(&ctx);
  graphics_flush();

  /* ---------------------------------------------------------------- */
  /* Event loop                                                        */
  /* ---------------------------------------------------------------- */
  bool    running    = true;
  int     ipc_status;
  message msg;

  while (running) {
    int r = driver_receive(ANY, &msg, &ipc_status);
    if (r != 0) continue;
    if (!is_ipc_notify(ipc_status)) continue;
    if (_ENDPOINT_P(msg.m_source) != HARDWARE) continue;

    const uint32_t ints = msg.m_notify.interrupts;

    /* ============================================================== */
    /* Keyboard                                                        */
    /* ============================================================== */
    if (ints & kbd_irq) {
      keyboard_ih();
      if (keyboard_has_data()) {
        uint8_t sc = keyboard_get_byte();
        keyboard_clear_data();

        AppState before = ctx.current;
        state->handle_key(&ctx, sc);
        if (!apply_transition(&ctx, before, &state)) running = false;
      }
    }

    /* ============================================================== */
    /* Mouse                                                           */
    /* ============================================================== */
    if (ints & mouse_irq) {
      mouse_ih();
      if (mouse_has_byte()) {
        uint8_t byte = mouse_get_byte();

        /* Synchronise on first byte (must have SYNC bit) */
        if (mouse_idx == 0 && !(byte & MOUSE_SYNC_BIT)) {
          /* Out of sync — discard */
        } else {
          mouse_bytes[mouse_idx++] = byte;

          if (mouse_idx == 3) {
            mouse_idx = 0;

            /* Skip overflow packets */
            if (!(mouse_bytes[0] & (MOUSE_X_OV | MOUSE_Y_OV))) {
              int16_t dx = (mouse_bytes[0] & MOUSE_X_SIGN)
                           ? (int16_t)(0xFF00u | mouse_bytes[1])
                           : (int16_t) mouse_bytes[1];
              int16_t dy = (mouse_bytes[0] & MOUSE_Y_SIGN)
                           ? (int16_t)(0xFF00u | mouse_bytes[2])
                           : (int16_t) mouse_bytes[2];

              bool lb = (mouse_bytes[0] & MOUSE_LB) != 0;

              /* Update absolute cursor (Y inverted: up = negative screen Y) */
              int nx = ctx.cursor_x + dx;
              int ny = ctx.cursor_y - dy;
              if (nx < 0) nx = 0;
              if (ny < 0) ny = 0;
              if (nx >= ctx.screen_w) nx = ctx.screen_w - 1;
              if (ny >= ctx.screen_h) ny = ctx.screen_h - 1;

              AppState before = ctx.current;
              state->handle_mouse(&ctx, nx, ny, lb);
              if (!apply_transition(&ctx, before, &state)) running = false;
            }
          }
        }
      }
    }

    /* ============================================================== */
    /* Timer                                                           */
    /* ============================================================== */
    if (ints & timer_irq) {
      game_timer_ih();

      AppState before = ctx.current;
      state->update(&ctx);
      if (!apply_transition(&ctx, before, &state)) {
        running = false;
      } else {
        state->draw(&ctx);
        graphics_flush();
      }
    }
  }

  /* ---------------------------------------------------------------- */
  /* Cleanup                                                           */
  /* ---------------------------------------------------------------- */
  scoreboard_free(&ctx.scoreboard);

  (void) proj_mouse_disable_reporting();
  (void) mouse_unsubscribe_int();
  (void) game_timer_unsubscribe_int();
  (void) keyboard_unsubscribe_int();

  /* Flush KBC output buffer to prevent keyboard freeze on exit */
  uint8_t stat, data;
  while (util_sys_inb(KBC_STAT_REG, &stat) == 0 && (stat & KBC_ST_OBF)) {
    util_sys_inb(KBC_OUT_BUF, &data);
    tickdelay(micros_to_ticks(20000));
  }

  if (vg_exit() != 0)
    printf("vg_exit failed\n");

  printf("Game over. Final score: %d\n", game_get_score(&ctx.game));
  return 0;
}
