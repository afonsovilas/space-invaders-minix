#ifndef SHIELD_H
#define SHIELD_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup shield Shield Entity
 * Defensive barriers that protect the player from alien fire.
 * @{
 */

/** Number of shields on screen at any time. */
#define SHIELD_COUNT 4

/**
 * @struct Shield
 * A single defensive shield barrier.
 */
typedef struct {
  int x, y;             /**< Position on screen */
  int width, height;    /**< Dimensions */
  bool alive;           /**< Whether this shield is still active */
} Shield;

/**
 * @struct ShieldArray
 * Container for all shields on screen.
 */
typedef struct {
  Shield shields[SHIELD_COUNT]; /**< Array of all shields */
} ShieldArray;

/**
 * Initializes all shields in their starting positions.
 *
 * @param shields Pointer to the ShieldArray to initialize
 * @return 0 on success, -1 if pointer is NULL
 */
int shield_array_init(ShieldArray *shields);

/**
 * Destroys a shield, removing it from play.
 *
 * @param shields      Pointer to the ShieldArray
 * @param shield_index Index of the shield to destroy
 */
void shield_destroy(ShieldArray *shields, int shield_index);

/**
 * Renders all active shields to the back-buffer.
 *
 * @param shields Pointer to the ShieldArray to render
 */
void shield_array_draw(const ShieldArray *shields);

/**
 * Erases a shield by overwriting it with background colour.
 *
 * @param shields           Pointer to the ShieldArray
 * @param shield_index      Index of the shield to erase
 * @param background_color  Background colour to fill with
 */
void shield_erase(const ShieldArray *shields, int shield_index,
                  uint32_t background_color);

/** @} */

#endif /* SHIELD_H */
