#include "game.h"
#include "graphics.h"
#include "keyboard.h"
#include "font.h"
#include "i8042.h"

#include <stddef.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Colour constants                                                    */
/* ------------------------------------------------------------------ */
#define BACKGROUND_COLOR 0x00u
#define HUD_BG_COLOR     0x01u   /* dark blue bar */
#define HUD_LINE_COLOR   0x09u   /* bright blue separator */
#define HUD_SCORE_COLOR  0x0Eu   /* yellow */
#define HUD_LIVES_COLOR  0x0Cu   /* red    */
#define CURSOR_COLOR     0x0Fu   /* white  */
#define CURSOR_DOT_COLOR 0x0Cu   /* red centre dot */

#define HUD_H 34   /* height of the HUD bar in pixels */

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

/** Rectangular AABB collision test. */
static bool check_collision(int x1, int y1, int w1, int h1,
                             int x2, int y2, int w2, int h2) {
  return x1 < x2 + w2 &&
         x1 + w1 > x2 &&
         y1 < y2 + h2 &&
         y1 + h1 > y2;
}

/**
 * Fire a player bullet if none is currently active.
 */
static void fire_player_bullet(Game *game) {
  if (game == NULL || game->state != GAME_STATE_PLAYING) return;
  if (!game->player_bullet.active) {
    bullet_fire_from_player(&game->player_bullet,
                            game->player.x, game->player.y,
                            game->player.width, game->player.height);
  }
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

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

int game_init(Game *game, int screen_width, int screen_height) {
  if (game == NULL) return -1;

  game->state         = GAME_STATE_MENU;
  game->screen_width  = screen_width;
  game->screen_height = screen_height;
  game->score         = 0;
  game->lives         = 3;
  game->moving_left   = false;
  game->moving_right  = false;

  /* Cursor starts centred */
  game->mouse.x       = screen_width  / 2;
  game->mouse.y       = screen_height / 2;
  game->mouse.lb      = false;
  game->mouse.rb      = false;
  game->mouse.mb      = false;
  game->mouse.lb_prev = false;

  if (player_init(&game->player)                 != 0) return -1;
  if (bullet_player_init(&game->player_bullet)   != 0) return -1;
  if (bullet_alien_init(&game->alien_bullet)     != 0) return -1;
  if (alien_swarm_init(&game->alien_swarm)       != 0) return -1;
  if (shield_array_init(&game->shields)          != 0) return -1;

  return 0;
}

void game_start(Game *game) {
  if (game == NULL || game->state != GAME_STATE_MENU) return;
  game->state = GAME_STATE_PLAYING;
}

void game_update(Game *game) {
  if (game == NULL || game->state != GAME_STATE_PLAYING) return;

  /* --- Player movement --- */
  if (game->moving_left)
    player_move_left(&game->player, game->screen_width);
  if (game->moving_right)
    player_move_right(&game->player, game->screen_width);

  /* --- Mouse left-click fires bullet (edge-triggered) --- */
  if (game->mouse.lb && !game->mouse.lb_prev)
    fire_player_bullet(game);
  game->mouse.lb_prev = game->mouse.lb;

  /* --- Player bullet --- */
  if (game->player_bullet.active) {
    if (!bullet_update_player(&game->player_bullet))
      game->player_bullet.active = false;
  }

  /* --- Alien bullet --- */
  if (game->alien_bullet.active) {
    if (!bullet_update_alien(&game->alien_bullet, game->screen_height))
      game->alien_bullet.active = false;
  }

  /* --- Alien bullet hits player --- */
  if (game->alien_bullet.active &&
      check_collision(game->alien_bullet.x,  game->alien_bullet.y,
                      game->alien_bullet.width, game->alien_bullet.height,
                      game->player.x,         game->player.y,
                      game->player.width,     game->player.height)) {
    game->lives--;
    game->alien_bullet.active = false;
    player_reset_position(&game->player);
    game->moving_left  = false;
    game->moving_right = false;
    if (game->lives <= 0) { game->state = GAME_STATE_PLAYER_LOST; return; }
  }

  /* --- Alien swarm movement --- */
  alien_swarm_update(&game->alien_swarm);

  if (alien_swarm_reached_player(&game->alien_swarm, game->player.y)) {
    game->state = GAME_STATE_PLAYER_LOST;
    return;
  }

  /* --- Alien fires --- */
  if (!game->alien_bullet.active) {
    int shooter;
    if (alien_swarm_get_shooter(&game->alien_swarm, &shooter))
      bullet_fire_from_alien(&game->alien_bullet,
                             game->alien_swarm.aliens[shooter].x,
                             game->alien_swarm.aliens[shooter].y,
                             game->alien_swarm.aliens[shooter].width,
                             game->alien_swarm.aliens[shooter].height);
  }

  /* --- Player bullet hits shield --- */
  if (game->player_bullet.active) {
    for (int i = 0; i < SHIELD_COUNT; i++) {
      if (!game->shields.shields[i].alive) continue;
      if (check_collision(game->player_bullet.x,     game->player_bullet.y,
                          game->player_bullet.width,  game->player_bullet.height,
                          game->shields.shields[i].x, game->shields.shields[i].y,
                          game->shields.shields[i].width, game->shields.shields[i].height)) {
        shield_destroy(&game->shields, i);
        game->player_bullet.active = false;
        break;
      }
    }
  }

  /* --- Alien bullet hits shield --- */
  if (game->alien_bullet.active) {
    for (int i = 0; i < SHIELD_COUNT; i++) {
      if (!game->shields.shields[i].alive) continue;
      if (check_collision(game->alien_bullet.x,      game->alien_bullet.y,
                          game->alien_bullet.width,   game->alien_bullet.height,
                          game->shields.shields[i].x, game->shields.shields[i].y,
                          game->shields.shields[i].width, game->shields.shields[i].height)) {
        shield_destroy(&game->shields, i);
        game->alien_bullet.active = false;
        break;
      }
    }
  }

  /* --- Player bullet hits alien --- */
  if (game->player_bullet.active) {
    for (int i = 0; i < ALIEN_COUNT; i++) {
      if (!game->alien_swarm.aliens[i].alive) continue;
      if (check_collision(game->player_bullet.x,           game->player_bullet.y,
                          game->player_bullet.width,        game->player_bullet.height,
                          game->alien_swarm.aliens[i].x,   game->alien_swarm.aliens[i].y,
                          game->alien_swarm.aliens[i].width, game->alien_swarm.aliens[i].height)) {
        alien_kill(&game->alien_swarm, i);
        game->player_bullet.active = false;
        game->score += 100;
        if (alien_swarm_all_dead(&game->alien_swarm)) {
          game->state = GAME_STATE_PLAYER_WON;
          return;
        }
        break;
      }
    }
  }
}

void game_draw(Game *game) {
  if (game == NULL) return;

  /* --- HUD bar --- */
  graphics_draw_rectangle(0, 0, (uint16_t) game->screen_width, HUD_H, HUD_BG_COLOR);
  graphics_draw_hline(0, HUD_H, (uint16_t) game->screen_width, HUD_LINE_COLOR);

  /* Score */
  char buf[20];
  build_hud_str("SCORE ", game->score, buf, (int) sizeof(buf));
  font_draw_string(10, 9, buf, HUD_SCORE_COLOR, 2);

  /* Lives */
  build_hud_str("LIVES ", game->lives, buf, (int) sizeof(buf));
  int lw = font_string_width(buf, 2);
  font_draw_string(game->screen_width - lw - 10, 9, buf, HUD_LIVES_COLOR, 2);

  /* --- Game objects --- */
  player_draw(&game->player);
  if (game->player_bullet.active) bullet_draw(&game->player_bullet);
  if (game->alien_bullet.active)  bullet_draw(&game->alien_bullet);
  alien_swarm_draw(&game->alien_swarm);
  shield_array_draw(&game->shields);
}

void game_draw_game_over(const Game *game) {
  if (game == NULL) return;

  bool won = (game->state == GAME_STATE_PLAYER_WON);

  /* Translucent overlay – fill with dark bg */
  graphics_clear(0x00u);

  /* Draw some stars for atmosphere */
  static const uint16_t GSTARS[][2] = {
    {50,40},{200,80},{400,30},{700,60},{900,20},{100,200},
    {350,150},{600,180},{850,130},{200,350},{500,300},{800,370},
    {150,500},{450,480},{750,520},{50,650},{900,680},{300,600},
  };
  for (int i = 0; i < 18; i++)
    graphics_draw_pixel(GSTARS[i][0], GSTARS[i][1], 0x0Fu);

  /* Screen border */
  graphics_draw_hline(0, 0, 1024, 0x0Bu);
  graphics_draw_hline(0, 1, 1024, 0x0Bu);

  /* Title */
  const char *title = won ? "YOU WIN" : "GAME OVER";
  uint32_t   tcol   = won ? 0x0Au    : 0x0Cu;   /* green : red */
  int tw = font_string_width(title, 5);
  font_draw_string(512 - tw / 2, 220, title, tcol, 5);

  /* Score */
  char sbuf[24];
  build_hud_str("FINAL SCORE ", game->score, sbuf, (int) sizeof(sbuf));
  int sw = font_string_width(sbuf, 3);
  font_draw_string(512 - sw / 2, 380, sbuf, 0x0Eu, 3);

  /* Exit hint */
  const char *hint = "PRESS ESC TO EXIT";
  int hw = font_string_width(hint, 2);
  font_draw_string(512 - hw / 2, 520, hint, 0x07u, 2);
}

void game_handle_keyboard_input(Game *game, uint8_t scancode, bool *running) {
  if (game == NULL || running == NULL) return;

  /* ESC always exits */
  if (scancode == KBD_ESC_BREAK) { *running = false; return; }

  if (game->state == GAME_STATE_MENU) {
    if (scancode == KBD_SPACE_MAKE) game_start(game);
    return;
  }

  if (game->state == GAME_STATE_PLAYING) {
    if      (scancode == KBD_A_MAKE)     game->moving_left  = true;
    else if (scancode == KBD_A_BREAK)    game->moving_left  = false;
    if      (scancode == KBD_D_MAKE)     game->moving_right = true;
    else if (scancode == KBD_D_BREAK)    game->moving_right = false;
    if      (scancode == KBD_SPACE_MAKE) fire_player_bullet(game);
  }
}

void game_handle_mouse_packet(Game *game, const uint8_t bytes[3]) {
  if (game == NULL || bytes == NULL) return;

  /* Skip overflow packets */
  if (bytes[0] & (MOUSE_X_OV | MOUSE_Y_OV)) return;

  /* Update button state */
  game->mouse.lb = (bytes[0] & MOUSE_LB) != 0;
  game->mouse.rb = (bytes[0] & MOUSE_RB) != 0;
  game->mouse.mb = (bytes[0] & MOUSE_MB) != 0;

  /* Compute signed deltas (sign-extend from 9-bit two's complement) */
  int16_t dx = (bytes[0] & MOUSE_X_SIGN) ? (int16_t)(0xFF00u | bytes[1]) : (int16_t)bytes[1];
  int16_t dy = (bytes[0] & MOUSE_Y_SIGN) ? (int16_t)(0xFF00u | bytes[2]) : (int16_t)bytes[2];

  /* Y is inverted: mouse moves up → screen Y decreases */
  game->mouse.x += dx;
  game->mouse.y -= dy;

  /* Clamp to screen */
  if (game->mouse.x < 0)                         game->mouse.x = 0;
  if (game->mouse.y < 0)                         game->mouse.y = 0;
  if (game->mouse.x >= game->screen_width)       game->mouse.x = game->screen_width  - 1;
  if (game->mouse.y >= game->screen_height)      game->mouse.y = game->screen_height - 1;
}

GameState game_get_state(const Game *game) {
  if (game == NULL) return GAME_STATE_MENU;
  return game->state;
}

int game_get_score(const Game *game) {
  if (game == NULL) return 0;
  return game->score;
}

int game_get_lives(const Game *game) {
  if (game == NULL) return 0;
  return game->lives;
}
