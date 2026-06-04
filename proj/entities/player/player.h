#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <stdint.h>
#include "bullet.h"
#include "scoreboard.h"

/**
 * @defgroup player Player Entity
 * The player-controlled spaceship with movement, shooting, and damage state.
 * @{
 */

/** Maximum lives the player starts with. */
#define MAX_LIVES 3

/**
 * @struct Player
 * Represents the player spaceship.
 * Tracks position, movement input, statistics (score/lives), and visual blink state.
 */
typedef struct {
  int x, y;                         /**< Current position */
  int width, height;                /**< Dimensions */
  int speed;                        /**< Movement speed (pixels/frame) */
  
  int initial_x, initial_y;         /**< Starting position (for reset) */
  
  int score;                        /**< Accumulated score */
  int lives;                        /**< Remaining lives */
  char name[SCOREBOARD_NAME_LEN];   /**< Player name from menu */
  
  bool moving_left;                 /**< Left key pressed */
  bool moving_right;                /**< Right key pressed */
  
  bool is_blinking;                 /**< Blinking animation active (after hit) */
  int  blink_timer;                 /**< Remaining blink animation frames */
  bool blink_show;                  /**< Whether sprite is visible this frame */
} Player;

/**
 * Initializes a Player with default starting values.
 *
 * @param player Pointer to the Player structure to initialize
 * @return 0 on success, -1 if pointer is NULL
 */
int player_init(Player *player);

/**
 * Moves the Player left, respecting screen boundaries.
 *
 * @param player        Pointer to the Player
 * @param screen_width  Screen width (used to enforce left boundary)
 */
void player_move_left(Player *player, int screen_width);

/**
 * Moves the Player right, respecting screen boundaries.
 *
 * @param player        Pointer to the Player
 * @param screen_width  Screen width (used to enforce right boundary)
 */
void player_move_right(Player *player, int screen_width);

/**
 * Renders the Player sprite to the back-buffer.
 *
 * @param player Pointer to the Player to render
 */
void player_draw(const Player *player);

/**
 * Erases the Player by overwriting it with background colour.
 *
 * @param player              Pointer to the Player
 * @param background_color    Background colour to fill with
 */
void player_erase(const Player *player, uint32_t background_color);

/**
 * Resets the Player to starting position.
 * Called when hit by an alien bullet.
 *
 * @param player Pointer to the Player
 */
void player_reset_position(Player *player);

/**
 * Resets movement and blink state.
 * Called at the start of each new wave.
 *
 * @param player Pointer to the Player
 */
void player_reset_state(Player *player);

/**
 * Updates the blink animation (called once per frame).
 * Decrements the blink timer and toggles visibility.
 *
 * @param player Pointer to the Player
 */
void player_update_blinking(Player *player);

/**
 * Checks if the Player sprite should be drawn this frame.
 * Returns false during the "hidden" phase of the blink animation.
 *
 * @param player Pointer to the Player
 * @return true if visible, false if hidden by blink animation
 */
bool player_is_visible(const Player *player);

/**
 * Processes a hit on the Player.
 * Decrements lives and starts the blink animation.
 * Should only be called when !player->is_blinking.
 * Caller must check player->lives <= 0 for game over.
 *
 * @param player Pointer to the Player
 */
void player_on_hit(Player *player);

/**
 * Bullet configuration for player shots.
 * Pass to bullet_init() when creating a player bullet.
 */
extern const BulletConfig PLAYER_BULLET_CONFIG;

/**
 * Fires a bullet from the Player.
 * Positions the bullet at the top-centre of the player.
 *
 * @param bullet Pointer to the Bullet to fire
 * @param player The Player that is shooting
 */
void player_fire_bullet(Bullet *bullet, const Player *player);

/** @} */

#endif /* PLAYER_H */
