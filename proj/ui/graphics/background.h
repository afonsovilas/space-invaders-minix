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

/** @} */

#endif /* BACKGROUND_H */
