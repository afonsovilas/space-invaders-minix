#include "game.h"
#include "sprites/heart_sprites.h"
#include "sprites/explosion_sprites.h"
#include "graphics.h"
#include "keyboard.h"
#include "font.h"
#include "scoreboard.h"

#include <stddef.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Colour & HUD constants                                              */
/* ------------------------------------------------------------------ */
#define BACKGROUND_COLOR 0x00u
#define HUD_BG_COLOR     0x01u   /* dark blue bar */
#define HUD_LINE_COLOR   0x09u   /* bright blue separator */
#define HUD_SCORE_COLOR  0x0Eu   /* yellow */
#define HUD_LIVES_COLOR  0x0Cu   /* red    */

#define HUD_H 34   /* height of the HUD bar in pixels */

static bool check_collision(int x1, int y1, int w1, int h1,
                             int x2, int y2, int w2, int h2) {
  return x1 < x2 + w2 &&
         x1 + w1 > x2 &&
         y1 < y2 + h2 &&
         y1 + h1 > y2;
}

/**
 * Convert a non-negative integer to a decimal ASCII string.
 * The output is right-filled with a null terminator.
 * buf must be at least 12 bytes.
 */
static void uint_to_str(unsigned int n, char *buf, int bufsize) {
  char tmp[12];
  int  i = 0;
  if (bufsize <= 0) return;
  if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
  while (n > 0 && i < 11) { tmp[i++] = (char)('0' + n % 10); n /= 10; }
  int j = 0;
  while (i > 0 && j < bufsize - 1) buf[j++] = tmp[--i];
  buf[j] = '\0';
}

/** Build "PREFIX N" into buf. */
static void build_hud_str(const char *prefix, int n, char *buf, int bufsize) {
  int pi = 0;
  while (prefix[pi] && pi < bufsize - 1) { buf[pi] = prefix[pi]; pi++; }
  uint_to_str((unsigned int)(n < 0 ? 0 : n), buf + pi, bufsize - pi);
}

static int game_max_active_alien_bullets(const Game *game) {
    if (game == NULL) {
        return 1;
    }

    /*
      Much faster bullet count scaling:
      Wave 1: 1 alien bullet
      Wave 2: 2 alien bullets
      Wave 3: 3 alien bullets
      Wave 4: 4 alien bullets
      ...
      capped at MAX_ALIEN_BULLETS.
    */
    int max_bullets = game->wave;

    if (max_bullets < 1) {
        max_bullets = 1;
    }

    if (max_bullets > MAX_ALIEN_BULLETS) {
        max_bullets = MAX_ALIEN_BULLETS;
    }

    return max_bullets;
}

static int game_alien_bullet_speed_for_wave(const Game *game) {
    if (game == NULL) {
        return ALIEN_BULLET_CONFIG.speed;
    }

    /*
      Faster bullet speed scaling:
      Wave 1: base speed
      Wave 2: +2
      Wave 3: +4
      Wave 4: +6
      etc.
    */
    int speed = ALIEN_BULLET_CONFIG.speed + (game->wave - 1) * 2;

    if (speed > 18) {
        speed = 18;
    }

    return speed;
}

/**
 * Called on every game-ending condition (win or loss).
 * Saves the score under the player's entered name and transitions to the
 * scoreboard screen.
 */
static void game_end(Game *game) {
  scoreboard_add(&game->scoreboard, game->player.name, game->player.score);
  game->state = GAME_STATE_SCOREBOARD;
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

int game_init(Game *game, int screen_width, int screen_height) {
  if (game == NULL) return -1;

  scoreboard_free(&game->scoreboard);

  game->state            = GAME_STATE_MENU;
  game->screen_width     = screen_width;
  game->screen_height    = screen_height;
  game->background_color = BACKGROUND_COLOR;

  game->wave             = 1;
  game->difficulty       = 1;

  /* Cursor starts centred */
  game->mouse.x          = screen_width  / 2;
  game->mouse.y          = screen_height / 2;
  game->mouse.lb         = false;
  game->mouse.rb         = false;
  game->mouse.mb         = false;
  game->mouse.lb_prev    = false;

  if (player_init(&game->player) != 0) return -1;

  for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
    if (bullet_init(&game->player_bullets[i], &PLAYER_BULLET_CONFIG) != 0) {
      return -1;
    }
  }

  for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
    if (bullet_init(&game->alien_bullets[i], &ALIEN_BULLET_CONFIG) != 0) {
        return -1;
    }
}
  if (alien_swarm_init(&game->alien_swarm)       != 0) return -1;
  alien_swarm_apply_difficulty(&game->alien_swarm, game->difficulty);
  if (shield_array_init(&game->shields)          != 0) return -1;

  for (int i = 0; i < MAX_EXPLOSIONS; i++) {
    game->explosions[i].active = false;
  }

  scoreboard_load(&game->scoreboard);

  return 0;
}

void game_start(Game *game) {
  if (game == NULL || game->state != GAME_STATE_MENU) return;
  game->state = GAME_STATE_PLAYING;
}

void game_next_wave(Game *game) {
    if (game == NULL) {
        return;
    }

    game->wave++;
    game->difficulty++;

    /*
      Clear all player bullets.
    */
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        game->player_bullets[i].active = false;
    }

    /*
      Clear all alien bullets.
    */
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        game->alien_bullets[i].active = false;
        game->alien_bullets[i].speed = game_alien_bullet_speed_for_wave(game);
    }

    player_reset_position(&game->player);
    player_reset_state(&game->player);

    /*
      New random alien formation.
    */
    alien_swarm_init(&game->alien_swarm);

    /*
      Apply stronger wave difficulty.
    */
    alien_swarm_apply_difficulty(&game->alien_swarm, game->difficulty);

    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        game->explosions[i].active = false;
    }
}



/* --- Funções Auxiliares de Atualização de Frame (SRP) --- */

static void game_spawn_explosion(Game *game, int x, int y, ExplosionType type, int width, int height) {
    if (game == NULL) return;
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!game->explosions[i].active) {
            game->explosions[i].active = true;
            game->explosions[i].x = x;
            game->explosions[i].y = y;
            game->explosions[i].width = width;
            game->explosions[i].height = height;
            game->explosions[i].frame = 0;
            game->explosions[i].type = type;
            break;
        }
    }
}

static void game_update_explosions(Game *game) {
    if (game == NULL) return;
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (game->explosions[i].active) {
            game->explosions[i].frame++;
            if (game->explosions[i].frame >= NUM_EXPLOSION_FRAMES * EXPLOSION_FRAME_DURATION) {
                game->explosions[i].active = false;
            }
        }
    }
}

static void game_update_positions(Game *game) {
    if (game->player.moving_left) {
        player_move_left(&game->player, game->screen_width);
    }

    if (game->player.moving_right) {
        player_move_right(&game->player, game->screen_width);
    }

    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (game->player_bullets[i].active) {
            if (!bullet_update(&game->player_bullets[i], game->screen_height)) {
                game->player_bullets[i].active = false;
            }
        }
    }

    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        if (game->alien_bullets[i].active) {
            if (!bullet_update(&game->alien_bullets[i], game->screen_height)) {
                game->alien_bullets[i].active = false;
            }
        }
    }
}

static void game_check_player_hit(Game *game) {
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        if (game->alien_bullets[i].active) {
            if (check_collision(game->alien_bullets[i].x, game->alien_bullets[i].y,
                                game->alien_bullets[i].width, game->alien_bullets[i].height,
                                game->player.x, game->player.y,
                                game->player.width, game->player.height)) {

                game->alien_bullets[i].active = false;

                if (!game->player.is_blinking) {
                    player_on_hit(&game->player);

                    if (game->player.lives <= 0) {
                        game_end(game);
                        return;
                    }
                }
            }
        }
    }
}

static void game_aliens_shoot(Game *game) {
    if (game == NULL) {
        return;
    }

    int max_bullets = game_max_active_alien_bullets(game);

    /*
      Count active alien bullets.
    */
    int active_bullets = 0;

    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        if (game->alien_bullets[i].active) {
            active_bullets++;
        }
    }

    if (active_bullets >= max_bullets) {
        return;
    }

    /*
      First shooter uses the normal shoot timer.
      If the timer says no shot yet, nobody shoots this frame.
    */
    int first_shooter_index;

    if (!alien_swarm_get_shooter(&game->alien_swarm, &first_shooter_index)) {
        return;
    }

    /*
      Number of bullets fired at the same time.
      Make this aggressive:
      Wave 1: 1 shot
      Wave 2: 2 shots
      Wave 3: 3 shots
      Wave 4+: 4 shots
    */
    int bullets_to_fire = game->wave;

    if (bullets_to_fire < 1) {
        bullets_to_fire = 1;
    }

    if (bullets_to_fire > 4) {
        bullets_to_fire = 4;
    }

    /*
      Fire the first timed shot.
    */
    for (int i = 0; i < max_bullets; i++) {
        if (!game->alien_bullets[i].active) {
            game->alien_bullets[i].speed = game_alien_bullet_speed_for_wave(game);

            alien_fire_bullet(&game->alien_bullets[i],
                              &game->alien_swarm.aliens[first_shooter_index]);

            active_bullets++;
            break;
        }
    }

    /*
      Fire extra simultaneous shots without waiting for shoot_counter again.
    */
    for (int shot = 1; shot < bullets_to_fire; shot++) {
        if (active_bullets >= max_bullets) {
            return;
        }

        int shooter_index;

        if (!alien_swarm_get_random_shooter_immediate(&game->alien_swarm, &shooter_index)) {
            return;
        }

        for (int i = 0; i < max_bullets; i++) {
            if (!game->alien_bullets[i].active) {
                game->alien_bullets[i].speed = game_alien_bullet_speed_for_wave(game);

                alien_fire_bullet(&game->alien_bullets[i],
                                  &game->alien_swarm.aliens[shooter_index]);

                active_bullets++;
                break;
            }
        }
    }
}

static void game_check_bullet_shield_collisions(Game *game,
                                                Bullet *bullets,
                                                int bullet_count) {
    for (int b = 0; b < bullet_count; b++) {
        if (bullets[b].active) {
            for (int i = 0; i < SHIELD_COUNT; i++) {
                if (game->shields.shields[i].alive) {
                    if (check_collision(bullets[b].x, bullets[b].y,
                                        bullets[b].width, bullets[b].height,
                                        game->shields.shields[i].x, game->shields.shields[i].y,
                                        game->shields.shields[i].width, game->shields.shields[i].height)) {
                        game_spawn_explosion(game, game->shields.shields[i].x, game->shields.shields[i].y,
                                             EXPLOSION_TYPE_SHIELD, 90, 30);
                        shield_erase(&game->shields, i, game->background_color);
                        shield_destroy(&game->shields, i);
                        bullets[b].active = false;
                        break;
                    }
                }
            }
        }
    }
}


static void game_check_player_bullet_alien_collision(Game *game) {
    for (int b = 0; b < MAX_PLAYER_BULLETS; b++) {
        if (game->player_bullets[b].active) {
            for (int i = 0; i < ALIEN_COUNT; i++) {
                if (game->alien_swarm.aliens[i].alive) {
                    if (check_collision(game->player_bullets[b].x, game->player_bullets[b].y,
                                       game->player_bullets[b].width, game->player_bullets[b].height,
                                       game->alien_swarm.aliens[i].x, game->alien_swarm.aliens[i].y,
                                       game->alien_swarm.aliens[i].width, game->alien_swarm.aliens[i].height)) {
                        game_spawn_explosion(game, game->alien_swarm.aliens[i].x, game->alien_swarm.aliens[i].y, EXPLOSION_TYPE_ALIEN, 40, 25);
                        alien_kill(&game->alien_swarm, i);
                        game->player_bullets[b].active = false;
                        game->player.score += 100;

                        if (alien_swarm_all_dead(&game->alien_swarm)) {
                            game_next_wave(game);
                            return;
                        }

                        break;
                    }
                }
            }
        }
    }
}

/* --- Função Principal de Atualização do Jogo --- */

void game_update(Game *game) {
    if (game == NULL || game->state != GAME_STATE_PLAYING) {
        return;
    }

    /* Mouse click fires bullet (edge-triggered) */
    if (game->mouse.lb && !game->mouse.lb_prev) {
        game_fire_player_bullet(game);
    }
    game->mouse.lb_prev = game->mouse.lb;

    player_update_blinking(&game->player);
    game_update_positions(game);
    game_update_explosions(game);

    game_check_player_hit(game);
    if (game->state != GAME_STATE_PLAYING) {
        return;
    }

    alien_swarm_update(&game->alien_swarm, game->screen_width);

    if (alien_swarm_reached_player(&game->alien_swarm, game->player.y)) {
        game_end(game);
        return;
    }

    game_aliens_shoot(game);

    game_check_bullet_shield_collisions(game, game->player_bullets, MAX_PLAYER_BULLETS);
    game_check_bullet_shield_collisions(game, game->alien_bullets,  MAX_ALIEN_BULLETS);
    game_check_player_bullet_alien_collision(game);
}

void game_draw(Game *game) {
  if (game == NULL) return;

  /* --- HUD bar --- */
  graphics_draw_rectangle(0, 0, (uint16_t) game->screen_width, HUD_H, HUD_BG_COLOR);
  graphics_draw_hline(0, HUD_H, (uint16_t) game->screen_width, HUD_LINE_COLOR);

  /* Score */
  char buf[20];
  build_hud_str("SCORE ", game->player.score, buf, (int) sizeof(buf));
  font_draw_string(10, 9, buf, HUD_SCORE_COLOR, 2);

  build_hud_str("WAVE ", game->wave, buf, (int) sizeof(buf));
  font_draw_string(250, 9, buf, 0x0Fu, 2);

  /* Lives (blinks when a life is lost / player is hit) */
  if (player_is_visible(&game->player)) {
      int heart_w = 21;
      int spacing = 5;
      int lives_y = 8;
      char lives_buf[10];
      build_hud_str(": ", game->player.lives, lives_buf, (int) sizeof(lives_buf));
      int text_w = font_string_width(lives_buf, 2);
      int start_x = game->screen_width - (heart_w + spacing + text_w) - 10;
      graphics_draw_xpm((xpm_map_t) HEART_SPRITE, (uint16_t) start_x, (uint16_t) lives_y);
      font_draw_string(start_x + heart_w + spacing, 9, lives_buf, HUD_LIVES_COLOR, 2);
  }

  /* --- Game objects --- */
  if (player_is_visible(&game->player)) {
    player_draw(&game->player);
  }

  for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
    if (game->player_bullets[i].active) {
      bullet_draw(&game->player_bullets[i]);
    }
  }

  for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
      if (game->alien_bullets[i].active) {
          bullet_draw(&game->alien_bullets[i]);
      }
  }

  alien_swarm_draw(&game->alien_swarm);
  shield_array_draw(&game->shields);

  for (int i = 0; i < MAX_EXPLOSIONS; i++) {
      if (game->explosions[i].active) {
          int frame_idx = game->explosions[i].frame / EXPLOSION_FRAME_DURATION;
          if (frame_idx < NUM_EXPLOSION_FRAMES) {
              if (game->explosions[i].type == EXPLOSION_TYPE_SHIELD) {
                  graphics_draw_xpm((xpm_map_t) SHIELD_EXPLOSION_SPRITES[frame_idx],
                                    (uint16_t) game->explosions[i].x,
                                    (uint16_t) game->explosions[i].y);
              } else {
                  graphics_draw_xpm((xpm_map_t) EXPLOSION_SPRITES[frame_idx],
                                    (uint16_t) game->explosions[i].x,
                                    (uint16_t) game->explosions[i].y);
              }
          }
      }
  }
}

void game_fire_player_bullet(Game *game) {
  if (game == NULL || game->state != GAME_STATE_PLAYING) {
    return;
  }

  for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
    if (!game->player_bullets[i].active) {
      player_fire_bullet(&game->player_bullets[i], &game->player);
      break;
    }
  }
}

void game_handle_keyboard_input(Game *game, uint8_t scancode, bool *running) {
  if (game == NULL || running == NULL) return;

  if (game->state == GAME_STATE_SCOREBOARD) {
    if (scancode == KBD_SPACE_MAKE) {
      game_init(game, game->screen_width, game->screen_height);
    } else if (scancode == KBD_ESC_BREAK) {
      *running = false;
    }
    return;
  }

  /* ESC always exits from any other state */
  if (scancode == KBD_ESC_BREAK) { *running = false; return; }

  if (game->state == GAME_STATE_MENU) {
    return;  /* keyboard shortcuts removed — menu handled by menu module */
  }

  if (game->state == GAME_STATE_PLAYING) {
    if      (scancode == KBD_A_MAKE)     game->player.moving_left  = true;
    else if (scancode == KBD_A_BREAK)    game->player.moving_left  = false;
    if      (scancode == KBD_D_MAKE)     game->player.moving_right = true;
    else if (scancode == KBD_D_BREAK)    game->player.moving_right = false;
    if      (scancode == KBD_SPACE_MAKE) game_fire_player_bullet(game);
  }
}

void game_draw_scoreboard(const Game *game) {
  if (game == NULL) return;
  graphics_clear(0x00u);
  scoreboard_draw(&game->scoreboard, game->screen_width, game->screen_height);
}

void game_enter_scoreboard(Game *game) {
  if (game == NULL) return;
  game->state = GAME_STATE_SCOREBOARD;
}

GameState game_get_state(const Game *game) {
  if (game == NULL) return GAME_STATE_MENU;
  return game->state;
}

int game_get_score(const Game *game) {
  if (game == NULL) return 0;
  return game->player.score;
}

int game_get_lives(const Game *game) {
  if (game == NULL) return 0;
  return game->player.lives;
}

int game_get_wave(const Game *game) {
  if (game == NULL) return 0;
  return game->wave;
}
