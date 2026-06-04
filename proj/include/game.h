#ifndef GAME_H
#define GAME_H

#include "alien.h"
#include "bullet.h"
#include "mouse.h"
#include "player.h"
#include "scoreboard.h"
#include "shield.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_PLAYER_BULLETS 3
#define MAX_ALIEN_BULLETS 8
#define MAX_LIVES 3

/**
 * @defgroup game Game
 * Core game state machine for Star Shooter.
 * @{
 */

/** Game state machine. */
typedef enum {
  GAME_STATE_MENU,
  GAME_STATE_PLAYING,
  GAME_STATE_PLAYER_WON,
  GAME_STATE_PLAYER_LOST,
  GAME_STATE_SCOREBOARD
} GameState;

/**
 * Root game structure – owns all game objects and state.
 */
typedef struct {
  GameState state;

  Player player;
  Bullet player_bullets[MAX_PLAYER_BULLETS];
  Bullet alien_bullets[MAX_ALIEN_BULLETS];

  AlienSwarm alien_swarm;
  ShieldArray shields;

  int score;
  int lives;

  int wave;
  int difficulty;

  char player_name[SCOREBOARD_NAME_LEN];

  Scoreboard scoreboard;

  uint32_t background_color;

  int screen_width;
  int screen_height;

  bool moving_left;
  bool moving_right;

  bool is_blinking;
  int blink_timer;
  bool blink_show;

  MouseState mouse;

} Game;

/** Initialise the game (does not start it — state = MENU). */
int game_init(Game *game, int screen_width, int screen_height);

/** Transition from MENU → PLAYING. */
void game_start(Game *game);

/** Starts the next alien wave after all aliens are cleared. */
void game_next_wave(Game *game);

/** Update game logic one tick. */
void game_update(Game *game);

/** Draw all game objects + HUD into the back-buffer. */
void game_draw(Game *game);

/** Erase all game objects. */
void game_erase(Game *game);

/** Draw game over screen. */
void game_draw_game_over(const Game *game);

/** Draw scoreboard screen. */
void game_draw_scoreboard(const Game *game);

/** Enter scoreboard state. */
void game_enter_scoreboard(Game *game);

/** Fires a bullet from the player if one is available. */
void game_fire_player_bullet(Game *game);

/** Process keyboard input. */
void game_handle_keyboard_input(Game *game, uint8_t scancode, bool *running);

/** Process mouse packet. */
void game_handle_mouse_packet(Game *game, const uint8_t bytes[3]);

/** Return current game state. */
GameState game_get_state(const Game *game);

/** Return current score. */
int game_get_score(const Game *game);

/** Return remaining lives. */
int game_get_lives(const Game *game);

/** Return current wave. */
int game_get_wave(const Game *game);

/**@}*/

#endif /* GAME_H */