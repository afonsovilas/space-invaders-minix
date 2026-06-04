#ifndef SPRITES_H
#define SPRITES_H

/* =====================================================================
 * sprites.h  –  XPM sprite data for all game entities.
 *
 * VBE mode 0x105 is 8-bit indexed (1 byte per pixel).
 *
 * The LCOM xpm_load() implementation for XPM_INDEXED mode uses a simplified
 * non-standard parser expecting exactly 3 header values and no 'c' in the
 * colour definitions.
 * ===================================================================*/

/* ---------------------------------------------------------------------
 * PLAYER_SPRITE  –  60 × 30 pixels, 3 colors
 * ------------------------------------------------------------------- */
static const char * const PLAYER_SPRITE[] = {
    "60 30 3",
    "  0",
    "G 10",       /* Light Green */
    "R 12",       /* Light Red */
    "                             RR                             ", /* row  0 */
    "                             RR                             ", /* row  1 */
    "                             RR                             ", /* row  2 */
    "                             RR                             ", /* row  3 */
    "                            GGGG                            ", /* row  4 */
    "                            GGGG                            ", /* row  5 */
    "                            GGGG                            ", /* row  6 */
    "                            GGGG                            ", /* row  7 */
    "                          GGGGGGGG                          ", /* row  8 */
    "                          GGGGGGGG                          ", /* row  9 */
    "                          GGGGGGGG                          ", /* row 10 */
    "                          GGGGGGGG                          ", /* row 11 */
    "                     GGGGGGGGGGGGGGGGGG                     ", /* row 12 */
    "                     GGGGGGGGGGGGGGGGGG                     ", /* row 13 */
    "                     GGGGGGGGGGGGGGGGGG                     ", /* row 14 */
    "                     GGGGGGGGGGGGGGGGGG                     ", /* row 15 */
    "                GGGGGGGGGGGGGGGGGGGGGGGGGGGG                ", /* row 16 */
    "                GGGGGGGGGGGGGGGGGGGGGGGGGGGG                ", /* row 17 */
    "                GGGGGGGGGGGGGGGGGGGGGGGGGGGG                ", /* row 18 */
    "                GGGGGGGGGGGGGGGGGGGGGGGGGGGG                ", /* row 19 */
    "         GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG         ", /* row 20 */
    "         GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG         ", /* row 21 */
    "         GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG         ", /* row 22 */
    "         GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG         ", /* row 23 */
    "    GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG    ", /* row 24 */
    "    GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG    ", /* row 25 */
    "    GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG    ", /* row 26 */
    "    GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG    ", /* row 27 */
    "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG", /* row 28 */
    "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG", /* row 29 */
};

/* ---------------------------------------------------------------------
 * ALIEN_SPRITE  –  40 × 25 pixels, 3 colors
 * ------------------------------------------------------------------- */
static const char * const ALIEN_SPRITE[] = {
    "40 25 3",
    "  0",
    "X 12",       /* Light Red */
    "O 14",       /* Yellow */
    "     XX                          XX     ", /* row  0 */
    "     XXXX                      XXXX     ", /* row  1 */
    "       XXXX                  XXXX       ", /* row  2 */
    "     XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX     ", /* row  3 */
    "   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX   ", /* row  4 */
    "   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX   ", /* row  5 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row  6 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row  7 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row  8 */
    " XXXXXXXX      XXXXXXXXXX      XXXXXXXX ", /* row  9 */
    " XXXXXXXX      XXXXXXXXXX      XXXXXXXX ", /* row 10 */
    " XXXXXXXX      XXXXXXXXXX      XXXXXXXX ", /* row 11 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row 12 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row 13 */
    "   XXXXXX  XXXXXXXXXXXXXXXXXX  XXXXXX   ", /* row 14 */
    "   XXXXXX  XXXXXXXXXXXXXXXXXX  XXXXXX   ", /* row 15 */
    "     XXXX  XXXXXX      XXXXXX  XXXX     ", /* row 16 */
    "             XXXX      XXXX             ", /* row 17 */
    "             XXXX      XXXX             ", /* row 18 */
    "           XXXX          XXXX           ", /* row 19 */
    "           XXXX          XXXX           ", /* row 20 */
    "         XXXX              XXXX         ", /* row 21 */
    "         XXXX              XXXX         ", /* row 22 */
    "       XXXX                  XXXX       ", /* row 23 */
    "       XXXX                  XXXX       ", /* row 24 */
};

/* ---------------------------------------------------------------------
 * BULLET_SPRITE  –  6 × 14 pixels, 3 colors
 * ------------------------------------------------------------------- */
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

/* ---------------------------------------------------------------------
 * SHIELD_SPRITE  –  90 × 30 pixels, 2 colors
 * ------------------------------------------------------------------- */
static const char * const SHIELD_SPRITE[] = {
    "90 30 2",
    "  0",
    "X 11",       /* Light Cyan */
    "              XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX              ", /* row  0 */
    "            XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX            ", /* row  1 */
    "          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX          ", /* row  2 */
    "        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX        ", /* row  3 */
    "      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX      ", /* row  4 */
    "    XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX    ", /* row  5 */
    "   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX   ", /* row  6 */
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  ", /* row  7 */
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  ", /* row  8 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row  9 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row 10 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row 11 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row 12 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row 13 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row 14 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row 15 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXXXX                              XXXXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row 16 */
    " XXXXXXXXXXXXXXXXXXXXXXXXXXX                                  XXXXXXXXXXXXXXXXXXXXXXXXXXX ", /* row 17 */
    " XXXXXXXXXXXXXXXXXXXXXXXXX                                      XXXXXXXXXXXXXXXXXXXXXXXXX ", /* row 18 */
    " XXXXXXXXXXXXXXXXXXXXXXX                                          XXXXXXXXXXXXXXXXXXXXXXX ", /* row 19 */
    " XXXXXXXXXXXXXXXXXXXXXX                                            XXXXXXXXXXXXXXXXXXXXXX ", /* row 20 */
    " XXXXXXXXXXXXXXXXXXXXX                                              XXXXXXXXXXXXXXXXXXXXX ", /* row 21 */
    " XXXXXXXXXXXXXXXXXXXX                                                XXXXXXXXXXXXXXXXXXXX ", /* row 22 */
    " XXXXXXXXXXXXXXXXXXX                                                  XXXXXXXXXXXXXXXXXXX ", /* row 23 */
    " XXXXXXXXXXXXXXXXXX                                                    XXXXXXXXXXXXXXXXXX ", /* row 24 */
    " XXXXXXXXXXXXXXXXX                                                      XXXXXXXXXXXXXXXXX ", /* row 25 */
    " XXXXXXXXXXXXXXXX                                                        XXXXXXXXXXXXXXXX ", /* row 26 */
    " XXXXXXXXXXXXXXX                                                          XXXXXXXXXXXXXXX ", /* row 27 */
    " XXXXXXXXXXXXXX                                                            XXXXXXXXXXXXXX ", /* row 28 */
    " XXXXXXXXXXXXX                                                              XXXXXXXXXXXXX ", /* row 29 */
};

#endif /* SPRITES_H */
