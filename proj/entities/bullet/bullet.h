#ifndef BULLET_H
#define BULLET_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup bullet Bullet Entity
 * Bullet projectiles for both player and aliens.
 * @{
 */

/**
 * @enum BulletDirection
 * Travel direction for a bullet.
 */
typedef enum {
  BULLET_DIR_UP   = -1,  /**< Upward (player bullet) */
  BULLET_DIR_DOWN =  1   /**< Downward (alien bullet) */
} BulletDirection;

/**
 * @struct BulletConfig
 * Configuration template for bullet initialization.
 * Each shooter type (player/alien) has its own configuration.
 */
typedef struct {
  int width;              /**< Bullet width in pixels */
  int height;             /**< Bullet height in pixels */
  int speed;              /**< Pixels per frame */
  uint32_t color;         /**< Render colour index */
  BulletDirection direction; /**< Travel direction */
} BulletConfig;

/** Maximum trail points for motion blur effect. */
#define BULLET_TRAIL_LENGTH 4

/**
 * @struct Bullet
 * A single bullet projectile (player or alien).
 * Tracks position, velocity, status, and motion trail for visual effects.
 */
typedef struct {
  int x, y;               /**< Current position */
  int width, height;      /**< Dimensions */
  int speed;              /**< Pixels per frame */
  uint32_t color;         /**< Render colour */
  bool active;            /**< Whether bullet is in flight */
  BulletDirection direction; /**< Movement direction */
  
  int trail_x[BULLET_TRAIL_LENGTH]; /**< Previous X positions for trail */
  int trail_y[BULLET_TRAIL_LENGTH]; /**< Previous Y positions for trail */
  int trail_count;                  /**< Number of valid trail points */
} Bullet;

/**
 * Initializes a Bullet from a configuration template.
 *
 * @param bullet Pointer to the Bullet to initialize
 * @param config Bullet configuration (properties and direction)
 * @return 0 on success, -1 if either pointer is NULL
 */
int bullet_init(Bullet *bullet, const BulletConfig *config);

/**
 * Updates Bullet position for one frame.
 * Deactivates the bullet if it leaves the screen.
 * Updates the trail for motion blur effect.
 *
 * @param bullet        Pointer to the Bullet to update
 * @param screen_height Screen height (used to check bottom boundary)
 * @return true if bullet is still active, false if deactivated by boundary check
 */
bool bullet_update(Bullet *bullet, int screen_height);

/**
 * Renders the Bullet sprite and motion trail to the back-buffer.
 *
 * @param bullet Pointer to the Bullet to render
 */
void bullet_draw(const Bullet *bullet);

/**
 * Erases the Bullet by overwriting it with background colour.
 *
 * @param bullet            Pointer to the Bullet to erase
 * @param background_color  Background colour to fill with
 */
void bullet_erase(const Bullet *bullet, uint32_t background_color);

/**
 * Deactivates a Bullet, removing it from play.
 *
 * @param bullet Pointer to the Bullet to deactivate
 */
void bullet_deactivate(Bullet *bullet);

/** @} */

#endif /* BULLET_H */
