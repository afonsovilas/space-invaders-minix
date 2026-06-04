#ifndef BULLET_SPRITES_H
#define BULLET_SPRITES_H

/**
 * @defgroup bullet_sprites Bullet Sprites
 * XPM sprite data for projectiles.
 * @{
 */

/** Bullet sprite (6×14 px, 3 colors). */
static const char * const BULLET_SPRITE[] = {
    "6 14 3",
    "  0",
    "X 14",       /* Yellow */
    "W 15",       /* White */
    "  XX  ", /* row  0 */
    " XXXX ", /* row  1 */
    " XXWW ", /* row  2 */
    " XXWW ", /* row  3 */
    " XXWW ", /* row  4 */
    " XXWW ", /* row  5 */
    " XXWW ", /* row  6 */
    " XXWW ", /* row  7 */
    " XXWW ", /* row  8 */
    " XXWW ", /* row  9 */
    " XXWW ", /* row 10 */
    " XXWW ", /* row 11 */
    " XXXX ", /* row 12 */
    "  XX  ", /* row 13 */
};

/** @} */

#endif /* BULLET_SPRITES_H */
