#ifndef GAME_H
#define GAME_H

#include "alien.h"
#include "bullet.h"
#include "mouse.h"
#include "player.h"
#include "shield.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_PLAYER_BULLETS 3
#define MAX_LIVES 3

/**
 * @defgroup game Game
 * Core game state machine for Star Shooter.
 * @{
 */

/** Game state machine. */
typedef enum {
  GAME_STATE_MENU,       /**< Main menu                */
  GAME_STATE_PLAYING,    /**< Game in progress         */
  GAME_STATE_PLAYER_WON, /**< Player destroyed all aliens */
  GAME_STATE_PLAYER_LOST /**< Player lost all lives    */
} GameState;

/**
 * Root game structure – owns all game objects and state.
 */
typedef struct {
  GameState state;

  Player player;
  Bullet player_bullets[MAX_PLAYER_BULLETS];
  Bullet alien_bullet;

  AlienSwarm alien_swarm;
  ShieldArray shields;

  int score;
  int lives;

  /* Cores */
  uint32_t background_color;

  /* Dimensões da tela */
  int screen_width;
  int screen_height;

  /* Flags de input */
  bool moving_left;
  bool moving_right;

  /* Blinking state */
  bool is_blinking;
  int blink_timer;
  bool blink_show;

  /* Mouse state */
  MouseState mouse;

} Game;

/** Initialise the game (does not start it — state = MENU). */
int game_init(Game *game, int screen_width, int screen_height);

/** Transition from MENU → PLAYING. */
void game_start(Game *game);

/** Update game logic one tick (collisions, movement, spawning). */
void game_update(Game *game);

/** Draw all game objects + HUD into the back-buffer. */
void game_draw(Game *game);

/** Erase all game objects (draw background color). */
void game_erase(Game *game);

/**
 * Draw a "game over" / "you win" screen.
 * Does NOT call graphics_flush() — caller must flush.
 */
void game_draw_game_over(const Game *game);

/** Fires a bullet from the player if one is available. */
void game_fire_player_bullet(Game *game);

/** Process a keyboard scancode (movement, fire, ESC, menu SPACE). */
void game_handle_keyboard_input(Game *game, uint8_t scancode, bool *running);

/**
 * Process a fully-assembled 3-byte mouse packet.
 * Updates game->mouse (position, buttons) and fires on left-click.
 * @param bytes  3-byte packet, bytes[0] is the sync byte.
 */
void game_handle_mouse_packet(Game *game, const uint8_t bytes[3]);

/** Return the current game state. */
GameState game_get_state(const Game *game);

/** Return the current score. */
int game_get_score(const Game *game);

/** Return remaining lives. */
int game_get_lives(const Game *game);

/**@}*/

#endif /* GAME_H */
