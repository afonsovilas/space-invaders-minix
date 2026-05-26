#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "graphics.h"
#include "keyboard.h"
#include "timer.h"

#define ALIEN_ROWS 3
#define ALIEN_COLS 8
#define ALIEN_COUNT (ALIEN_ROWS * ALIEN_COLS)

#define SHIELD_COUNT 4

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

static bool all_aliens_dead(bool alien_alive[]) {
  for (int i = 0; i < ALIEN_COUNT; i++) {
    if (alien_alive[i]) {
      return false;
    }
  }

  return true;
}

int(proj_main_loop)(int argc, char *argv[]) {
  printf("Opening graphics mode...\n");

  uint16_t mode = 0x105;

  if (graphics_init(mode) != 0) {
    printf("graphics_init failed\n");
    return 1;
  }

  int player_x = 100;
  int player_y = 500;
  int player_width = 60;
  int player_height = 30;
  int player_speed = 5;

  bool bullet_active = false;
  int bullet_x = 0;
  int bullet_y = 0;
  int bullet_width = 6;
  int bullet_height = 14;
  int bullet_speed = 8;

  bool alien_bullet_active = false;
  int alien_bullet_x = 0;
  int alien_bullet_y = 0;
  int alien_bullet_width = 6;
  int alien_bullet_height = 14;
  int alien_bullet_speed = 5;

  int alien_shoot_counter = 0;
  int alien_shoot_interval = 120;
  int alien_shoot_column = 0;

  int alien_x[ALIEN_COUNT];
  int alien_y[ALIEN_COUNT];
  bool alien_alive[ALIEN_COUNT];

  int alien_width = 40;
  int alien_height = 25;
  int alien_x_spacing = 20;
  int alien_y_spacing = 20;

  int alien_speed = 2;
  int alien_direction = 1;
  int alien_drop = 20;

  int alien_frame_counter = 0;
  int alien_move_interval = 10;

  int shield_x[SHIELD_COUNT];
  int shield_y[SHIELD_COUNT];
  bool shield_alive[SHIELD_COUNT];

  int shield_width = 90;
  int shield_height = 30;

  uint32_t background_color = 0x00;
  uint32_t player_color = 0x2A;
  uint32_t bullet_color = 0x3F;
  uint32_t alien_color = 0x1C;
  uint32_t alien_bullet_color = 0x3C;
  uint32_t shield_color = 0x02;

  bool moving_left = false;
  bool moving_right = false;

  bool game_started = false;

  bool player_won = false;
  bool player_lost = false;

  int score = 0;
  int lives = 3;

  for (int row = 0; row < ALIEN_ROWS; row++) {
    for (int col = 0; col < ALIEN_COLS; col++) {
      int i = row * ALIEN_COLS + col;

      alien_x[i] = 100 + col * (alien_width + alien_x_spacing);
      alien_y[i] = 80 + row * (alien_height + alien_y_spacing);
      alien_alive[i] = true;
    }
  }

  for (int i = 0; i < SHIELD_COUNT; i++) {
    shield_x[i] = 120 + i * 220;
    shield_y[i] = 400;
    shield_alive[i] = true;
  }

  /*
    Start screen.
    Blue rectangle = start menu.
    Press SPACE to start the game.
  */
  graphics_draw_rectangle(300, 250, 400, 150, 0x09);

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
              if (!game_started) {
                game_started = true;

                /*
                  Erase start screen.
                */
                graphics_draw_rectangle(300, 250, 400, 150, background_color);

                /*
                  Draw initial game objects.
                */
                graphics_draw_rectangle(player_x, player_y, player_width, player_height, player_color);

                for (int i = 0; i < ALIEN_COUNT; i++) {
                  if (alien_alive[i]) {
                    graphics_draw_rectangle(alien_x[i], alien_y[i], alien_width, alien_height, alien_color);
                  }
                }

                for (int i = 0; i < SHIELD_COUNT; i++) {
                  if (shield_alive[i]) {
                    graphics_draw_rectangle(shield_x[i], shield_y[i], shield_width, shield_height, shield_color);
                  }
                }
              }

              else if (!bullet_active) {
                bullet_active = true;

                bullet_x = player_x + player_width / 2 - bullet_width / 2;
                bullet_y = player_y - bullet_height;
              }
            }
          }
        }

        if (msg.m_notify.interrupts & timer_irq_set) {
          game_timer_ih();

          if (!game_started) {
            continue;
          }

          /*
            Erase old objects.
          */
          graphics_draw_rectangle(player_x, player_y, player_width, player_height, background_color);

          if (bullet_active) {
            graphics_draw_rectangle(bullet_x, bullet_y, bullet_width, bullet_height, background_color);
          }

          if (alien_bullet_active) {
            graphics_draw_rectangle(alien_bullet_x, alien_bullet_y,
                                    alien_bullet_width, alien_bullet_height,
                                    background_color);
          }

          for (int i = 0; i < ALIEN_COUNT; i++) {
            if (alien_alive[i]) {
              graphics_draw_rectangle(alien_x[i], alien_y[i], alien_width, alien_height, background_color);
            }
          }

          for (int i = 0; i < SHIELD_COUNT; i++) {
            if (shield_alive[i]) {
              graphics_draw_rectangle(shield_x[i], shield_y[i], shield_width, shield_height, background_color);
            }
          }

          /*
            Update player position.
          */
          if (moving_left && player_x >= player_speed) {
            player_x -= player_speed;
          }

          if (moving_right && player_x + player_width + player_speed < 1024) {
            player_x += player_speed;
          }

          /*
            Update player bullet.
          */
          if (bullet_active) {
            if (bullet_y >= bullet_speed) {
              bullet_y -= bullet_speed;
            } else {
              bullet_active = false;
            }
          }

          /*
            Update alien bullet.
          */
          if (alien_bullet_active) {
            if (alien_bullet_y + alien_bullet_height + alien_bullet_speed < 768) {
              alien_bullet_y += alien_bullet_speed;
            } else {
              alien_bullet_active = false;
            }
          }

          /*
            Check alien bullet-player collision.
          */
          if (alien_bullet_active) {
            if (check_collision(alien_bullet_x, alien_bullet_y,
                                alien_bullet_width, alien_bullet_height,
                                player_x, player_y,
                                player_width, player_height)) {
              lives--;

              alien_bullet_active = false;

              graphics_draw_rectangle(alien_bullet_x, alien_bullet_y,
                                      alien_bullet_width, alien_bullet_height,
                                      background_color);

              graphics_draw_rectangle(player_x, player_y, player_width, player_height, background_color);

              player_x = 100;
              player_y = 500;

              moving_left = false;
              moving_right = false;

              if (lives <= 0) {
                player_lost = true;
                running = false;
              }
            }
          }

          /*
            Update alien movement.
          */
          alien_frame_counter++;

          if (alien_frame_counter >= alien_move_interval) {
            bool should_change_direction = false;

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
            Check lose condition: aliens reached player area.
          */
          for (int i = 0; i < ALIEN_COUNT; i++) {
            if (alien_alive[i]) {
              if (alien_y[i] + alien_height >= player_y) {
                player_lost = true;
                running = false;
                break;
              }
            }
          }

          /*
            Alien shooting: lowest alive alien in rotating column shoots.
          */
          alien_shoot_counter++;

          if (!alien_bullet_active && alien_shoot_counter >= alien_shoot_interval) {
            bool shot_fired = false;

            for (int attempt = 0; attempt < ALIEN_COLS && !shot_fired; attempt++) {
              int col = (alien_shoot_column + attempt) % ALIEN_COLS;

              for (int row = ALIEN_ROWS - 1; row >= 0; row--) {
                int i = row * ALIEN_COLS + col;

                if (alien_alive[i]) {
                  alien_bullet_active = true;

                  alien_bullet_x = alien_x[i] + alien_width / 2 - alien_bullet_width / 2;
                  alien_bullet_y = alien_y[i] + alien_height;

                  alien_shoot_column = (col + 1) % ALIEN_COLS;
                  shot_fired = true;

                  break;
                }
              }
            }

            alien_shoot_counter = 0;
          }

          /*
            Player bullet-shield collision.
          */
          if (bullet_active) {
            for (int i = 0; i < SHIELD_COUNT; i++) {
              if (shield_alive[i]) {
                if (check_collision(bullet_x, bullet_y, bullet_width, bullet_height,
                                    shield_x[i], shield_y[i],
                                    shield_width, shield_height)) {
                  shield_alive[i] = false;
                  bullet_active = false;
                  break;
                }
              }
            }
          }

          /*
            Alien bullet-shield collision.
          */
          if (alien_bullet_active) {
            for (int i = 0; i < SHIELD_COUNT; i++) {
              if (shield_alive[i]) {
                if (check_collision(alien_bullet_x, alien_bullet_y,
                                    alien_bullet_width, alien_bullet_height,
                                    shield_x[i], shield_y[i],
                                    shield_width, shield_height)) {
                  shield_alive[i] = false;
                  alien_bullet_active = false;
                  break;
                }
              }
            }
          }

          /*
            Player bullet-alien collision.
          */
          if (bullet_active) {
            for (int i = 0; i < ALIEN_COUNT; i++) {
              if (alien_alive[i]) {
                if (check_collision(bullet_x, bullet_y, bullet_width, bullet_height,
                                    alien_x[i], alien_y[i],
                                    alien_width, alien_height)) {
                  alien_alive[i] = false;
                  bullet_active = false;

                  score += 100;

                  graphics_draw_rectangle(alien_x[i], alien_y[i],
                                          alien_width, alien_height,
                                          background_color);

                  if (all_aliens_dead(alien_alive)) {
                    player_won = true;
                    running = false;
                  }

                  break;
                }
              }
            }
          }

          /*
            Draw current objects.
          */
          graphics_draw_rectangle(player_x, player_y, player_width, player_height, player_color);

          if (bullet_active) {
            graphics_draw_rectangle(bullet_x, bullet_y, bullet_width, bullet_height, bullet_color);
          }

          if (alien_bullet_active) {
            graphics_draw_rectangle(alien_bullet_x, alien_bullet_y,
                                    alien_bullet_width, alien_bullet_height,
                                    alien_bullet_color);
          }

          for (int i = 0; i < ALIEN_COUNT; i++) {
            if (alien_alive[i]) {
              graphics_draw_rectangle(alien_x[i], alien_y[i], alien_width, alien_height, alien_color);
            }
          }

          for (int i = 0; i < SHIELD_COUNT; i++) {
            if (shield_alive[i]) {
              graphics_draw_rectangle(shield_x[i], shield_y[i], shield_width, shield_height, shield_color);
            }
          }
        }
      }
    }
  }

  /*
    Cleanup immediately after the game loop ends.
    No end-screen drawing here, because that was making Minix stay visually stuck.
  */
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

  if (player_won) {
    printf("You win!\n");
  }

  if (player_lost) {
    printf("You lose!\n");
  }

  printf("Final score: %d\n", score);
  printf("Lives remaining: %d\n", lives);

  printf("Exited graphics mode.\n");

  return 0;
}