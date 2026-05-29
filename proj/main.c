#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "game.h"
#include "graphics.h"
#include "keyboard.h"
#include "timer.h"
#include "mouse.h"
#include "menu.h"
#include "i8042.h"

/* ------------------------------------------------------------------ */
/* Crosshair cursor – drawn in game mode after game_draw()            */
/* ------------------------------------------------------------------ */

static void draw_crosshair(int cx, int cy) {
  /* Horizontal arms (gap of ±3 px around centre) */
  if (cx > 12)
    graphics_draw_hline((uint16_t)(cx - 10), (uint16_t) cy, 7, 0x0Fu);
  if (cx < 1011)
    graphics_draw_hline((uint16_t)(cx + 4),  (uint16_t) cy, 7, 0x0Fu);

  /* Vertical arms */
  for (int dy = -10; dy <= 10; dy++) {
    if (dy >= -2 && dy <= 2) continue;
    int py = cy + dy;
    if (py >= 0 && py < 768)
      graphics_draw_pixel((uint16_t) cx, (uint16_t) py, 0x0Fu);
  }

  /* Centre dot */
  graphics_draw_rectangle((uint16_t)(cx - 1), (uint16_t)(cy - 1), 3, 3, 0x0Cu);
}

/* ------------------------------------------------------------------ */
/* Main entry points                                                   */
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

  /* ---- Initialise graphics (VBE mode 0x105: 1024×768, 8-bit) ---- */
  if (graphics_init(0x105) != 0) {
    printf("graphics_init failed\n");
    return 1;
  }

  /* ---- Initialise game state ---- */
  Game game;
  if (game_init(&game, 1024, 768) != 0) {
    printf("game_init failed\n");
    vg_exit();
    return 1;
  }

  /* ---- Initialise menu ---- */
  Menu menu;
  menu_init(&menu);

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

  /* Set mouse to stream mode and enable data reporting */
  if (mouse_setup_stream() != 0)
    printf("mouse_setup_stream failed (continuing)\n");

  uint32_t kbd_irq   = BIT(kbd_bit);
  uint32_t timer_irq = BIT(timer_bit);
  uint32_t mouse_irq = BIT(mouse_bit);

  /* ---- Mouse packet accumulator ---- */
  uint8_t mouse_bytes[3];
  uint8_t mouse_idx = 0;

  /* ---- Absolute cursor position (shared between menu and game) ---- */
  int cursor_x = 512;
  int cursor_y = 384;

  /* ---- Draw initial menu frame ---- */
  menu_draw(&menu);
  graphics_flush();

  /* ---------------------------------------------------------------- */
  /* Event loop                                                        */
  /* ---------------------------------------------------------------- */
  bool running = true;
  int  ipc_status;
  message msg;

  while (running) {
    int r = driver_receive(ANY, &msg, &ipc_status);
    if (r != 0) continue;
    if (!is_ipc_notify(ipc_status)) continue;
    if (_ENDPOINT_P(msg.m_source) != HARDWARE) continue;

    uint32_t ints = msg.m_notify.interrupts;

    /* ============================================================== */
    /* Keyboard interrupt                                              */
    /* ============================================================== */
    if (ints & kbd_irq) {
      keyboard_ih();
      if (keyboard_has_data()) {
        uint8_t sc = keyboard_get_byte();
        keyboard_clear_data();
        game_handle_keyboard_input(&game, sc, &running);
      }
    }

    /* ============================================================== */
    /* Mouse interrupt                                                 */
    /* ============================================================== */
    if (ints & mouse_irq) {
      mouse_ih();
      if (mouse_has_byte()) {
        uint8_t byte = mouse_get_byte();

        /* Synchronise on first byte: must have SYNC bit set */
        if (mouse_idx == 0 && !(byte & MOUSE_SYNC_BIT)) {
          /* Out of sync – discard */
        } else {
          mouse_bytes[mouse_idx++] = byte;

          if (mouse_idx == 3) {
            mouse_idx = 0;

            /* Skip overflow packets */
            if (!(mouse_bytes[0] & (MOUSE_X_OV | MOUSE_Y_OV))) {

              /* Parse delta (sign-extend from 9-bit two's complement) */
              int16_t dx = (mouse_bytes[0] & MOUSE_X_SIGN)
                           ? (int16_t)(0xFF00u | mouse_bytes[1])
                           : (int16_t) mouse_bytes[1];
              int16_t dy = (mouse_bytes[0] & MOUSE_Y_SIGN)
                           ? (int16_t)(0xFF00u | mouse_bytes[2])
                           : (int16_t) mouse_bytes[2];

              bool lb = (mouse_bytes[0] & MOUSE_LB) != 0;

              /* Update shared cursor (Y inverted: up = negative screen Y) */
              cursor_x += dx;
              cursor_y -= dy;
              if (cursor_x < 0)    cursor_x = 0;
              if (cursor_y < 0)    cursor_y = 0;
              if (cursor_x >= 1024) cursor_x = 1023;
              if (cursor_y >= 768)  cursor_y = 767;

              GameState state = game_get_state(&game);

              if (state == GAME_STATE_MENU) {
                menu_update(&menu, cursor_x, cursor_y, lb);
              } else {
                /* Propagate cursor + button state to game */
                game.mouse.x  = cursor_x;
                game.mouse.y  = cursor_y;
                game.mouse.lb = lb;
                game.mouse.rb = (mouse_bytes[0] & MOUSE_RB) != 0;
              }
            }
          }
        }
      }
    }

    /* ============================================================== */
    /* Timer interrupt                                                 */
    /* ============================================================== */
    if (ints & timer_irq) {
      game_timer_ih();

      GameState state = game_get_state(&game);

      if (state == GAME_STATE_MENU) {
        /* Check if the menu produced an action */
        MenuAction action = menu_get_action(&menu);
        if (action == MENU_ACTION_PLAY) {
          game_start(&game);
        } else if (action == MENU_ACTION_QUIT) {
          running = false;
        }

        /* Redraw menu every timer tick */
        menu_draw(&menu);
        graphics_flush();

      } else if (state == GAME_STATE_PLAYING) {
        graphics_clear(0x00u);
        game_update(&game);
        game_draw(&game);
        draw_crosshair(game.mouse.x, game.mouse.y);
        graphics_flush();

      } else {
        /* PLAYER_WON or PLAYER_LOST – show end screen until ESC */
        game_draw_game_over(&game);
        graphics_flush();
      }
    }
  }

  /* ---- Cleanup ---- */
  (void) proj_mouse_disable_reporting();
  (void) mouse_unsubscribe_int();
  (void) game_timer_unsubscribe_int();
  (void) keyboard_unsubscribe_int();

  /* Flush the KBC output buffer to prevent keyboard freezing on exit */
  uint8_t stat, data;
  while (util_sys_inb(KBC_STAT_REG, &stat) == 0 && (stat & KBC_ST_OBF)) {
    util_sys_inb(KBC_OUT_BUF, &data);
    tickdelay(micros_to_ticks(20000));
  }

  if (vg_exit() != 0)
    printf("vg_exit failed\n");

  printf("Game over. Final score: %d\n", game_get_score(&game));
  return 0;
}
