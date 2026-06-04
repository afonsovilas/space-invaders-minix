#ifndef HEART_SPRITES_H
#define HEART_SPRITES_H

/**
 * @defgroup heart_sprites Heart Sprites
 * XPM sprite data for lives/health indicator.
 * @{
 */

/** Heart sprite (21×17 px, 2 colors). */
static const char * const HEART_SPRITE[] = {
    "21 17 2",
    "  0",
    "R 12",       /* Light Red */
    "                     ", /* row  0 */
    "    RRRR     RRRR    ", /* row  1 */
    "  RRRRRRRR RRRRRRRR  ", /* row  2 */
    " RRRRRRRRRRRRRRRRRRR ", /* row  3 */
    " RRRRRRRRRRRRRRRRRRR ", /* row  4 */
    " RRRRRRRRRRRRRRRRRRR ", /* row  5 */
    " RRRRRRRRRRRRRRRRRRR ", /* row  6 */
    "  RRRRRRRRRRRRRRRRR  ", /* row  7 */
    "   RRRRRRRRRRRRRRR   ", /* row  8 */
    "    RRRRRRRRRRRRR    ", /* row  9 */
    "     RRRRRRRRRRR     ", /* row 10 */
    "      RRRRRRRRR      ", /* row 11 */
    "       RRRRRRR       ", /* row 12 */
    "        RRRRR        ", /* row 13 */
    "         RRR         ", /* row 14 */
    "          R          ", /* row 15 */
    "                     ", /* row 16 */
};

/** @} */

#endif /* HEART_SPRITES_H */
