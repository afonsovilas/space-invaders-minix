#ifndef GAME_H
#define GAME_H

#include "alien.h"
#include "bullet.h"
#include "player.h"
#include "shield.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup game Game Engine
 * Self-contained game simulation (Star Shooter).
 * Manages all entities: player, bullets, aliens, shields, explosions.
 * Does not own scoreboard, application state, or input handling.
 * @{
 */

/** Maximum number of player bullets on screen. */
#define MAX_PLAYER_BULLETS 3

/** Maximum number of alien bullets on screen. */
#define MAX_ALIEN_BULLETS 8

/** Maximum number of active explosions on screen. */
#define MAX_EXPLOSIONS 15

/** Frames each explosion animation frame is displayed. */
#define EXPLOSION_FRAME_DURATION 3

/** Number of frames in an explosion animation. */
#define NUM_EXPLOSION_FRAMES 4

/**
 * @enum ExplosionType
 * Type of explosion to render.
 */
typedef enum {
  EXPLOSION_TYPE_ALIEN,   /**< Alien explosion */
  EXPLOSION_TYPE_SHIELD   /**< Shield explosion */
} ExplosionType;

/**
 * @struct Explosion
 * A single active explosion animation.
 */
typedef struct {
  int x, y;               /**< Position on screen */
  int width, height;      /**< Dimensions */
  int frame;              /**< Current animation frame */
  bool active;            /**< Whether this explosion is playing */
  ExplosionType type;     /**< Animation type to display */
} Explosion;

/**
 * @struct Game
 * Main game simulation state.
 * Owns all entities, wave counter, and difficulty level.
 */
typedef struct {
  Player player;                             /**< Player spaceship */
  Bullet player_bullets[MAX_PLAYER_BULLETS]; /**< Player projectiles */
  Bullet alien_bullets[MAX_ALIEN_BULLETS];   /**< Alien projectiles */
  
  AlienSwarm alien_swarm;                    /**< Alien formation */
  ShieldArray shields;                       /**< Shield barriers */
  
  int wave;                                  /**< Current wave number */
  int difficulty;                            /**< Current difficulty level */
  bool over;                                 /**< Game over flag */
  
  uint32_t background_color;                 /**< Background colour */
  int screen_width, screen_height;           /**< Screen resolution */
  
  Explosion explosions[MAX_EXPLOSIONS];      /**< Active explosions */
} Game;

/**
 * Initializes a new game session.
 * Safe to call multiple times; does not modify scoreboard.
 *
 * @param game        Pointer to Game to initialize
 * @param screen_w    Screen width in pixels
 * @param screen_h    Screen height in pixels
 * @return 0 on success, non-zero on failure
 */
int game_init(Game *game, int screen_w, int screen_h);

/**
 * Advances game simulation one frame.
 * Updates all entities and checks for end conditions.
 *
 * @param game Pointer to Game to update
 * @return true if game is still running, false if game over
 */
bool game_update(Game *game);

/**
 * Renders all game objects and HUD to the back-buffer.
 * Does not call graphics_flush(); caller must do that.
 *
 * @param game Pointer to Game to render
 */
void game_draw(const Game *game);

/**
 * Fires a player bullet from the current position.
 * Does nothing if no bullet slot is available.
 *
 * @param game Pointer to Game
 */
void game_fire_player_bullet(Game *game);

/**
 * Advances to the next wave.
 * Called internally when all aliens are eliminated.
 *
 * @param game Pointer to Game
 */
void game_next_wave(Game *game);

/**
 * Returns the current player score.
 *
 * @param game Pointer to Game
 * @return Current score
 */
int game_get_score(const Game *game);

/**
 * Returns the player's remaining lives.
 *
 * @param game Pointer to Game
 * @return Remaining lives
 */
int game_get_lives(const Game *game);

/**
 * Returns the current wave number.
 *
 * @param game Pointer to Game
 * @return Wave number
 */
int game_get_wave(const Game *game);

/** @} */

#endif /* GAME_H */
