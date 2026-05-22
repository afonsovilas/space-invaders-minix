#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "graphics.h"
#include "keyboard.h"
#include "timer.h"

#define ALIEN_COUNT 8

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

static bool check_collision(int x1, int y1, int w1, int h1,
                            int x2, int y2, int w2, int h2) {
  return x1 < x2 + w2 &&
         x1 + w1 > x2 &&
         y1 < y2 + h2 &&
         y1 + h1 > y2;
}

int(proj_main_loop)(int argc, char *argv[]) {
  printf("Opening graphics mode...\n");

  uint16_t mode = 0x105;

  if (graphics_init(mode) != 0) {
    printf("graphics_init failed\n");
    return 1;
  }

  /*
    Player variables
  */
  int player_x = 100;
  int player_y = 500;
  int player_width = 60;
  int player_height = 30;
  int player_speed = 5;

  /*
    Bullet variables
  */
  bool bullet_active = false;
  int bullet_x = 0;
  int bullet_y = 0;
  int bullet_width = 6;
  int bullet_height = 14;
  int bullet_speed = 8;

  /*
    Alien variables
  */
  int alien_x[ALIEN_COUNT];
  int alien_y[ALIEN_COUNT];
  bool alien_alive[ALIEN_COUNT];

  int alien_width = 40;
  int alien_height = 25;
  int alien_spacing = 20;

  int alien_speed = 2;
  int alien_direction = 1; /* 1 = right, -1 = left */
  int alien_drop = 20;

  /*
    We do not move aliens every timer tick, because that would be too fast.
    Instead, we count frames and only move them every few timer interrupts.
  */
  int alien_frame_counter = 0;
  int alien_move_interval = 10;

  /*
    Colors
  */
  uint32_t background_color = 0x00;
  uint32_t player_color = 0x2A;
  uint32_t bullet_color = 0x3F;
  uint32_t alien_color = 0x1C;

  /*
    Movement state
  */
  bool moving_left = false;
  bool moving_right = false;

  /*
    Initialize aliens in one row
  */
  for (int i = 0; i < ALIEN_COUNT; i++) {
    alien_x[i] = 100 + i * (alien_width + alien_spacing);
    alien_y[i] = 100;
    alien_alive[i] = true;
  }

  /*
    Draw initial player
  */
  graphics_draw_rectangle(player_x, player_y, player_width, player_height, player_color);

  /*
    Draw initial aliens
  */
  for (int i = 0; i < ALIEN_COUNT; i++) {
    if (alien_alive[i]) {
      graphics_draw_rectangle(alien_x[i], alien_y[i], alien_width, alien_height, alien_color);
    }
  }

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

  while (running) {
    int r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0) {
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {

        /*
          Keyboard interrupt
        */
        if (msg.m_notify.interrupts & keyboard_irq_set) {
          keyboard_ih();

          if (keyboard_has_data()) {
            uint8_t scancode = keyboard_get_byte();
            keyboard_clear_data();

            if (scancode == KBD_ESC_BREAK) {
              running = false;
            }

            else if (scancode == KBD_A_MAKE) {
              moving_left = true;
            }

            else if (scancode == KBD_A_BREAK) {
              moving_left = false;
            }

            else if (scancode == KBD_D_MAKE) {
              moving_right = true;
            }

            else if (scancode == KBD_D_BREAK) {
              moving_right = false;
            }

            else if (scancode == KBD_SPACE_MAKE) {
              if (!bullet_active) {
                bullet_active = true;

                bullet_x = player_x + player_width / 2 - bullet_width / 2;
                bullet_y = player_y - bullet_height;
              }
            }
          }
        }

        /*
          Timer interrupt
        */
        if (msg.m_notify.interrupts & timer_irq_set) {
          game_timer_ih();

          /*
            Erase old player
          */
          graphics_draw_rectangle(player_x, player_y, player_width, player_height, background_color);

          /*
            Erase old bullet
          */
          if (bullet_active) {
            graphics_draw_rectangle(bullet_x, bullet_y, bullet_width, bullet_height, background_color);
          }

          /*
            Erase old aliens
          */
          for (int i = 0; i < ALIEN_COUNT; i++) {
            if (alien_alive[i]) {
              graphics_draw_rectangle(alien_x[i], alien_y[i], alien_width, alien_height, background_color);
            }
          }

          /*
            Update player position
          */
          if (moving_left && player_x >= player_speed) {
            player_x -= player_speed;
          }

          if (moving_right && player_x + player_width + player_speed < 1024) {
            player_x += player_speed;
          }

          /*
            Update bullet position
          */
          if (bullet_active) {
            if (bullet_y >= bullet_speed) {
              bullet_y -= bullet_speed;
            } else {
              bullet_active = false;
            }
          }

          /*
            Update alien position
          */
          alien_frame_counter++;

          if (alien_frame_counter >= alien_move_interval) {
            bool should_change_direction = false;

            /*
              Check if any alive alien is about to hit the side of the screen.
            */
            for (int i = 0; i < ALIEN_COUNT; i++) {
              if (alien_alive[i]) {
                if (alien_direction == 1 &&
                    alien_x[i] + alien_width + alien_speed >= 1024) {
                  should_change_direction = true;
                }

                if (alien_direction == -1 &&
                    alien_x[i] - alien_speed <= 0) {
                  should_change_direction = true;
                }
              }
            }

            if (should_change_direction) {
              alien_direction *= -1;

              for (int i = 0; i < ALIEN_COUNT; i++) {
                if (alien_alive[i]) {
                  alien_y[i] += alien_drop;
                }
              }
            } else {
              for (int i = 0; i < ALIEN_COUNT; i++) {
                if (alien_alive[i]) {
                  alien_x[i] += alien_speed * alien_direction;
                }
              }
            }

            alien_frame_counter = 0;
          }

          /*
            Check bullet-alien collisions
          */
          if (bullet_active) {
            for (int i = 0; i < ALIEN_COUNT; i++) {
              if (alien_alive[i]) {
                if (check_collision(bullet_x, bullet_y, bullet_width, bullet_height,
                                    alien_x[i], alien_y[i], alien_width, alien_height)) {
                  alien_alive[i] = false;
                  bullet_active = false;

                  graphics_draw_rectangle(alien_x[i], alien_y[i], alien_width, alien_height, background_color);

                  break;
                }
              }
            }
          }

          /*
            Draw player
          */
          graphics_draw_rectangle(player_x, player_y, player_width, player_height, player_color);

          /*
            Draw bullet
          */
          if (bullet_active) {
            graphics_draw_rectangle(bullet_x, bullet_y, bullet_width, bullet_height, bullet_color);
          }

          /*
            Draw aliens
          */
          for (int i = 0; i < ALIEN_COUNT; i++) {
            if (alien_alive[i]) {
              graphics_draw_rectangle(alien_x[i], alien_y[i], alien_width, alien_height, alien_color);
            }
          }
        }
      }
    }
  }

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

  printf("Exited graphics mode.\n");

  return 0;
}