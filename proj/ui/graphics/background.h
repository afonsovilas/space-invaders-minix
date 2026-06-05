#ifndef BACKGROUND_H
#define BACKGROUND_H

/**
 * @defgroup background Background
 * Shared background drawing routines for UI and Game state.
 * @{
 */

/**
 * Draws the shared starfield background directly to the back buffer.
 * It clears the screen using the background color, draws the starfield,
 * and renders top and bottom borders.
 */
void draw_starfield_background(void);

/**
 * Draws the starfield background and a small Earth sprite at the bottom center.
 * Used exclusively during gameplay to make it look like the ship is defending Earth.
 */
void draw_game_background(void);

/** @} */

#endif /* BACKGROUND_H */
