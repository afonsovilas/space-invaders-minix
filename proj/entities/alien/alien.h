#ifndef ALIEN_H
#define ALIEN_H

#include <stdint.h>
#include <stdbool.h>
#include "bullet.h"

/**
 * @defgroup alien Alien Entity
 * Enemy aliens in a swarm formation that descends toward the player.
 * @{
 */

/** Number of rows in the alien swarm formation. */
#define ALIEN_ROWS 6

/** Number of columns in the alien swarm formation. */
#define ALIEN_COLS 8

/** Total number of aliens in a swarm. */
#define ALIEN_COUNT (ALIEN_ROWS * ALIEN_COLS)

/**
 * @struct Alien
 * A single alien enemy.
 */
typedef struct {
  int x, y;               /**< Position on screen */
  int width, height;      /**< Dimensions */
  bool alive;             /**< Whether this alien is still in play */
} Alien;

/**
 * @struct AlienSwarm
 * Manages all aliens as a coordinated group.
 * Handles formation movement, difficulty scaling, and shooting mechanics.
 */
typedef struct {
  Alien aliens[ALIEN_COUNT]; /**< Array of all aliens */
  int speed;                  /**< Base movement speed */
  int direction;              /**< Direction: 1 (right), -1 (left) */
  int drop;                   /**< Pixels to descend when direction reverses */
  int frame_counter;          /**< Countdown for next movement */
  int move_interval;          /**< Frames between each movement step */
  int shoot_counter;          /**< Countdown for next alien shot */
  int shoot_interval;         /**< Frames between alien shots */
} AlienSwarm;

/**
 * Initializes a swarm with all aliens in formation.
 *
 * @param swarm Pointer to the AlienSwarm to initialize
 * @return 0 on success, -1 if pointer is NULL
 */
int alien_swarm_init(AlienSwarm *swarm);

/**
 * Adjusts swarm speed and shooting rate based on difficulty level.
 * Higher difficulty = faster movement and more frequent shots.
 *
 * @param swarm      Pointer to the AlienSwarm
 * @param difficulty Difficulty level (typically 0–N, higher is harder)
 */
void alien_swarm_apply_difficulty(AlienSwarm *swarm, int difficulty);

/**
 * Updates swarm position and shooting logic for one frame.
 * Moves formation horizontally/vertically and handles reverse-direction drops.
 *
 * @param swarm        Pointer to the AlienSwarm
 * @param screen_width Screen width (used to detect right boundary)
 */
void alien_swarm_update(AlienSwarm *swarm, int screen_width);

/**
 * Checks if all aliens in the swarm are dead.
 *
 * @param swarm Pointer to the AlienSwarm
 * @return true if all aliens are dead, false if any are still alive
 */
bool alien_swarm_all_dead(const AlienSwarm *swarm);

/**
 * Checks if any alien has reached the player's vertical level.
 *
 * @param swarm    Pointer to the AlienSwarm
 * @param player_y Player Y position (vertical threshold)
 * @return true if any alien's Y ≥ player_y, false otherwise
 */
bool alien_swarm_reached_player(const AlienSwarm *swarm, int player_y);

/**
 * Selects an alien to fire (the lowest alive alien in a rotating column).
 * Uses an internal column counter to cycle through columns for balanced coverage.
 *
 * @param swarm        Pointer to the AlienSwarm
 * @param alien_index  Output parameter: index of the selected alien
 * @return true if an alive alien was found, false if all aliens are dead
 */
bool alien_swarm_get_shooter(AlienSwarm *swarm, int *alien_index);

/**
 * Immediately selects a random alive alien to fire.
 * Unlike alien_swarm_get_shooter, does not cycle through columns.
 *
 * @param swarm       Pointer to the AlienSwarm
 * @param alien_index Output parameter: index of the selected alien
 * @return true if an alive alien was found, false if all aliens are dead
 */
bool alien_swarm_get_random_shooter_immediate(const AlienSwarm *swarm, int *alien_index);

/**
 * Marks an alien as dead.
 *
 * @param swarm       Pointer to the AlienSwarm
 * @param alien_index Index of the alien to kill
 */
void alien_kill(AlienSwarm *swarm, int alien_index);

/**
 * Renders all alive aliens in the swarm to the back-buffer.
 *
 * @param swarm Pointer to the AlienSwarm
 */
void alien_swarm_draw(const AlienSwarm *swarm);

/**
 * Erases all aliens by overwriting with background colour.
 *
 * @param swarm            Pointer to the AlienSwarm
 * @param background_color Background colour to fill with
 */
void alien_swarm_erase(const AlienSwarm *swarm, uint32_t background_color);

/**
 * Bullet configuration for alien shots.
 * Pass to bullet_init() when creating an alien bullet.
 */
extern const BulletConfig ALIEN_BULLET_CONFIG;

/**
 * Fires a bullet from an alien.
 * Positions the bullet at the bottom-centre of the alien.
 *
 * @param bullet Pointer to the Bullet to fire
 * @param alien  The Alien shooting the bullet
 */
void alien_fire_bullet(Bullet *bullet, const Alien *alien);

/** @} */

#endif /* ALIEN_H */
