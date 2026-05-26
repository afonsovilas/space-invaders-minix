#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "game.h"
#include "graphics.h"
#include "keyboard.h"
#include "timer.h"

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");

  lcf_trace_calls("/home/lcom/labs/grupo_2leic02_1/proj/trace.txt");
  lcf_log_output("/home/lcom/labs/grupo_2leic02_1/proj/output.txt");

  if (lcf_start(argc, argv)) {
    return 1;
  }

  lcf_cleanup();

  return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {
  printf("Opening graphics mode...\n");

  uint16_t mode = 0x105;

  if (graphics_init(mode) != 0) {
    printf("graphics_init failed\n");
    return 1;
  }

  /* Inicializa o estado do jogo (1024x768) */
  Game game;
  if (game_init(&game, 1024, 768) != 0) {
    printf("game_init failed\n");
    vg_exit();
    return 1;
  }

  /* Subscreve interrupções */
  uint8_t keyboard_bit_no;
  uint8_t timer_bit_no;

  if (keyboard_subscribe_int(&keyboard_bit_no) != 0) {
    printf("keyboard_subscribe_int failed\n");
    vg_exit();
    return 1;
  }

  if (game_timer_subscribe_int(&timer_bit_no) != 0) {
    printf("game_timer_subscribe_int failed\n");
    keyboard_unsubscribe_int();
    vg_exit();
    return 1;
  }

  uint32_t keyboard_irq_set = BIT(keyboard_bit_no);
  uint32_t timer_irq_set = BIT(timer_bit_no);

  int ipc_status;
  message msg;

  bool running = true;

  /* Desenha o menu inicial */
  graphics_draw_rectangle(300, 250, 400, 150, 0x09);

  while (running) {
    int r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0) {
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {

        /* Interrupção do teclado */
        if (msg.m_notify.interrupts & keyboard_irq_set) {
          keyboard_ih();

          if (keyboard_has_data()) {
            uint8_t scancode = keyboard_get_byte();
            keyboard_clear_data();

            /* Processa o input no módulo de jogo */
            if (game_get_state(&game) == GAME_STATE_MENU) {
              /* No menu, apaga o retângulo quando o jogo começa */
              if (scancode == 0x39) { /* SPACE_MAKE */
                graphics_draw_rectangle(300, 250, 400, 150, 0x00);
              }
            }

            game_handle_keyboard_input(&game, scancode, &running);
          }
        }

        /* Interrupção do timer */
        if (msg.m_notify.interrupts & timer_irq_set) {
          game_timer_ih();

          /* Só atualiza e desenha se o jogo está em progresso */
          if (game_get_state(&game) == GAME_STATE_PLAYING) {
            game_update(&game);
            game_draw(&game);
          }

          /* Verifica condições de fim de jogo */
          if (game_get_state(&game) == GAME_STATE_PLAYER_WON ||
              game_get_state(&game) == GAME_STATE_PLAYER_LOST) {
            running = false;
          }
        }
      }
    }
  }

  /* Unsubscreve interrupções */
  if (game_timer_unsubscribe_int() != 0) {
    printf("game_timer_unsubscribe_int failed\n");
  }

  if (keyboard_unsubscribe_int() != 0) {
    printf("keyboard_unsubscribe_int failed\n");
  }

  if (vg_exit() != 0) {
    printf("vg_exit failed\n");
    return 1;
  }

  /* Imprime resultado final */
  GameState final_state = game_get_state(&game);

  if (final_state == GAME_STATE_PLAYER_WON) {
    printf("You win!\n");
  } else if (final_state == GAME_STATE_PLAYER_LOST) {
    printf("You lose!\n");
  }

  printf("Final score: %d\n", game_get_score(&game));
  printf("Lives remaining: %d\n", game_get_lives(&game));

  printf("Exited graphics mode.\n");

  return 0;
}
