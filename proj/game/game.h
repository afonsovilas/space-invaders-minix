#ifndef GAME_H
#define GAME_H

#include "alien.h"
#include "bullet.h"
#include "player.h"
#include "shield.h"
#include <stdbool.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Limits & animation constants                                        */
/* ------------------------------------------------------------------ */

#define MAX_PLAYER_BULLETS     3
#define MAX_ALIEN_BULLETS      8
#define MAX_EXPLOSIONS         15
#define EXPLOSION_FRAME_DURATION 3
#define NUM_EXPLOSION_FRAMES   4

/* ------------------------------------------------------------------ */
/* Explosion                                                           */
/* ------------------------------------------------------------------ */

/** Type of explosion animation. */
typedef enum {
  EXPLOSION_TYPE_ALIEN,
  EXPLOSION_TYPE_SHIELD
} ExplosionType;

/** One active explosion instance. */
typedef struct {
  int  x, y;
  int  width, height;
  int  frame;
  bool active;
  ExplosionType type;
} Explosion;

/* ------------------------------------------------------------------ */
/* Game  (pure simulation – no state machine, no UI, no mouse)        */
/* ------------------------------------------------------------------ */

/**
 * @defgroup game Game
 * Self-contained game simulation for Star Shooter.
 *
 * Owns all gameplay entities (player, bullets, aliens, shields,
 * explosions) and the wave/difficulty counters.  It does NOT own
 * the scoreboard, the application state, or any input state.
 * @{
 */
typedef struct {
  Player  player;
  Bullet  player_bullets[MAX_PLAYER_BULLETS];
  Bullet  alien_bullets[MAX_ALIEN_BULLETS];

  AlienSwarm alien_swarm;
  ShieldArray shields;

  int  wave;
  int  difficulty;
  bool over;               /**< Set to true when the game ends (internal) */

  uint32_t background_color;
  int      screen_width;
  int      screen_height;

  Explosion explosions[MAX_EXPLOSIONS];
} Game;

/**
 * Initialise (or re-initialise) all game entities for a new session.
 * Safe to call multiple times; does not touch the scoreboard.
 */
int  game_init(Game *game, int screen_w, int screen_h);

/**
 * Advance game logic one tick.
 * @return true  – simulation is still running
 * @return false – game has ended (player dead or aliens reached player)
 */
bool game_update(Game *game);

/** Draw all game objects + HUD into the back-buffer (does not flush). */
void game_draw(const Game *game);

/** Fire a player bullet from the current player position, if one is free. */
void game_fire_player_bullet(Game *game);

/** Advance to the next wave; called internally when all aliens are killed. */
void game_next_wave(Game *game);

/** Return current score. */
int  game_get_score(const Game *game);

/** Return remaining lives. */
int  game_get_lives(const Game *game);

/** Return current wave number. */
int  game_get_wave(const Game *game);

/**@}*/

#endif /* GAME_H */
