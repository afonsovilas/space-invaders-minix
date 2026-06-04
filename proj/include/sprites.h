#ifndef SPRITES_H
#define SPRITES_H

/* =====================================================================
 * sprites.h     XPM sprite data for all game entities.
 *
 * VBE mode 0x105 is 8-bit indexed (1 byte per pixel).
 *
 * The LCOM xpm_load() implementation for XPM_INDEXED mode uses a simplified
 * non-standard parser expecting exactly 3 header values and no 'c' in the
 * colour definitions.
 * ===================================================================*/

/* ---------------------------------------------------------------------
 * PLAYER_SPRITE     60 x 30 pixels, 3 colors
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
 * ALIEN_SPRITE     40 x 25 pixels, 3 colors
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
 * BULLET_SPRITE     6 x 14 pixels, 3 colors
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
 * SHIELD_SPRITE     90 x 30 pixels, 2 colors
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

/* ---------------------------------------------------------------------
 * EXPLOSION_SPRITE_1  -  40 x 25 pixels, 3 colors
 * ------------------------------------------------------------------- */
static const char * const EXPLOSION_SPRITE_1[] = {
    "40 25 3",
    "  0",
    "X 12",       /* Light Red */
    "O 14",       /* Yellow */
    "                                        ", /* row  0 */
    "                                        ", /* row  1 */
    "                                        ", /* row  2 */
    "                                        ", /* row  3 */
    "                                        ", /* row  4 */
    "                                        ", /* row  5 */
    "                                        ", /* row  6 */
    "                                        ", /* row  7 */
    "                                        ", /* row  8 */
    "                  XX                    ", /* row  9 */
    "                 XXXX                   ", /* row 10 */
    "                XXOOXX                  ", /* row 11 */
    "                XOOOOX                  ", /* row 12 */
    "                XXOOXX                  ", /* row 13 */
    "                 XXXX                   ", /* row 14 */
    "                  XX                    ", /* row 15 */
    "                                        ", /* row 16 */
    "                                        ", /* row 17 */
    "                                        ", /* row 18 */
    "                                        ", /* row 19 */
    "                                        ", /* row 20 */
    "                                        ", /* row 21 */
    "                                        ", /* row 22 */
    "                                        ", /* row 23 */
    "                                        ", /* row 24 */
};

/* ---------------------------------------------------------------------
 * EXPLOSION_SPRITE_2  -  40 x 25 pixels, 3 colors
 * ------------------------------------------------------------------- */
static const char * const EXPLOSION_SPRITE_2[] = {
    "40 25 3",
    "  0",
    "X 12",       /* Light Red */
    "O 14",       /* Yellow */
    "                                        ", /* row  0 */
    "                                        ", /* row  1 */
    "                                        ", /* row  2 */
    "                                        ", /* row  3 */
    "                                        ", /* row  4 */
    "                  X                     ", /* row  5 */
    "                X X X                   ", /* row  6 */
    "               XX O XX                  ", /* row  7 */
    "              XXX O XXX                 ", /* row  8 */
    "             XXXXOOOXXXX                ", /* row  9 */
    "             XXOOOOOOOXX                ", /* row 10 */
    "           X OOOOOOOOOOO X              ", /* row 11 */
    "          XX OOOOOOOOOOO XX             ", /* row 12 */
    "           X OOOOOOOOOOO X              ", /* row 13 */
    "             XXOOOOOOOXX                ", /* row 14 */
    "             XXXXOOOXXXX                ", /* row 15 */
    "              XXX O XXX                 ", /* row 16 */
    "               XX O XX                  ", /* row 17 */
    "                X X X                   ", /* row 18 */
    "                  X                     ", /* row 19 */
    "                                        ", /* row 20 */
    "                                        ", /* row 21 */
    "                                        ", /* row 22 */
    "                                        ", /* row 23 */
    "                                        ", /* row 24 */
};

/* ---------------------------------------------------------------------
 * EXPLOSION_SPRITE_3  -  40 x 25 pixels, 3 colors
 * ------------------------------------------------------------------- */
static const char * const EXPLOSION_SPRITE_3[] = {
    "40 25 3",
    "  0",
    "X 12",       /* Light Red */
    "O 14",       /* Yellow */
    "                                        ", /* row  0 */
    "                                        ", /* row  1 */
    "                  X                     ", /* row  2 */
    "             X    X    X                ", /* row  3 */
    "              X  XXX  X                 ", /* row  4 */
    "            X  XXOOOXX  X               ", /* row  5 */
    "             XX OOOOO XX                ", /* row  6 */
    "            XX OOOOOOO XX               ", /* row  7 */
    "          XX  OOOOOOOOO  XX             ", /* row  8 */
    "         XX  OOXXOOOXXOO  XX            ", /* row  9 */
    "        XX  OOXX OOO XXOO  XX           ", /* row 10 */
    "       X   OOX   OOO   XOO   X          ", /* row 11 */
    "      XXX OOO     O     OOO XXX         ", /* row 12 */
    "       X   OOX   OOO   XOO   X          ", /* row 13 */
    "        XX  OOXX OOO XXOO  XX           ", /* row 14 */
    "         XX  OOXXOOOXXOO  XX            ", /* row 15 */
    "          XX  OOOOOOOOO  XX             ", /* row 16 */
    "            XX OOOOOOO XX               ", /* row 17 */
    "             XX OOOOO XX                ", /* row 18 */
    "            X  XXOOOXX  X               ", /* row 19 */
    "              X  XXX  X                 ", /* row 20 */
    "             X    X    X                ", /* row 21 */
    "                  X                     ", /* row 22 */
    "                                        ", /* row 23 */
    "                                        ", /* row 24 */
};

/* ---------------------------------------------------------------------
 * EXPLOSION_SPRITE_4  -  40 x 25 pixels, 3 colors
 * ------------------------------------------------------------------- */
static const char * const EXPLOSION_SPRITE_4[] = {
    "40 25 3",
    "  0",
    "X 12",       /* Light Red */
    "O 14",       /* Yellow */
    "                  X                     ", /* row  0 */
    "                                        ", /* row  1 */
    "             X         X                ", /* row  2 */
    "                                        ", /* row  3 */
    "         X       X       X              ", /* row  4 */
    "                                        ", /* row  5 */
    "     X       X       X       X          ", /* row  6 */
    "                                        ", /* row  7 */
    "         X   O       O   X              ", /* row  8 */
    "                                        ", /* row  9 */
    "  X   O                     O   X       ", /* row 10 */
    "                                        ", /* row 11 */
    "    O                         O         ", /* row 12 */
    "                                        ", /* row 13 */
    "  X   O                     O   X       ", /* row 14 */
    "                                        ", /* row 15 */
    "         X   O       O   X              ", /* row 16 */
    "                                        ", /* row 17 */
    "     X       X       X       X          ", /* row 18 */
    "                                        ", /* row 19 */
    "         X       X       X              ", /* row 20 */
    "                                        ", /* row 21 */
    "             X         X                ", /* row 22 */
    "                                        ", /* row 23 */
    "                  X                     ", /* row 24 */
};

static const char * const * const EXPLOSION_SPRITES[] = {
    EXPLOSION_SPRITE_1,
    EXPLOSION_SPRITE_2,
    EXPLOSION_SPRITE_3,
    EXPLOSION_SPRITE_4
};

static const char * const SHIELD_EXPLOSION_SPRITE_1[] = {
    "90 30 3",
    "  0",
    "X 11",       /* Light Cyan */
    "O 9",        /* Bright Blue */
    "                                                                                          ", /* row  0 */
    "                                                                                          ", /* row  1 */
    "                                X             X       X                                   ", /* row  2 */
    "                         X    X                  X        X X                             ", /* row  3 */
    "                        X         X       XX    X       X         X                       ", /* row  4 */
    "                                X  X        X     X        X  X X   X                     ", /* row  5 */
    "               XX       X            X     X X    X  X    X X                             ", /* row  6 */
    "             X                 X  O  OOX O     X  OOOO OX                                 ", /* row  7 */
    "                     X       O  O OO OXO   OXXO    OO    O O       X    X                 ", /* row  8 */
    "          X       X  X    X XXOOO   OXO   X OO  XO      OO    X                           ", /* row  9 */
    "          X         X    XOOOOOO      OXOX  OOOO OOXOO OOX  O        X    X       X       ", /* row 10 */
    "          X                  OX OO     XXOXXOOOXOXX    O X      O  O                      ", /* row 11 */
    "                      O O     O   O OXOXXXOXXOOOXOXXOO O              X  X        X       ", /* row 12 */
    "          X X           OO        OOOOXXXXXXOXOXXOOXXXXXOO  OOOXOO    X X     X X         ", /* row 13 */
    "           XX  X X     OX O O   OOOXOOXOXOOXXXXOOOOXXXOXXOOO  O     X X                   ", /* row 14 */
    "      X            XXOO  X    O  OXXXXXOXOXXOXXXOXOXOOXOO    X O O O    X                 ", /* row 15 */
    "     X X           X   O   X    O XXXOXOXXOOXXOOOOXOOOOO O O    XXXXX                     ", /* row 16 */
    "      X             XO  X  X    X  OXOOXXXXXXXOOOXXXXOO  X X OO  O  O          XX         ", /* row 17 */
    "                   X   OO XO OOXOOOX  XXOXXXXXXXXOXX O O O   X OOOOO      X X             ", /* row 18 */
    "              X X    X      O O  O  OX   XO    O   O XXXOO XO XOOO  X X  XX    X          ", /* row 19 */
    "             X              OOX   OX               X OOO O OO  O    X      X              ", /* row 20 */
    "                                 O  OOXOOOOOO  XO O     O X                               ", /* row 21 */
    "                           X     X O  OO  O OO  O O       X                               ", /* row 22 */
    "                X                              X   X   X        X     XX                  ", /* row 23 */
    "                              X                  X             X                          ", /* row 24 */
    "                                                  XX                                      ", /* row 25 */
    "                         X    X   X    X    X X X          X X                            ", /* row 26 */
    "                                        X    X  XX   X    X                               ", /* row 27 */
    "                                                                                          ", /* row 28 */
    "                                                                                          ", /* row 29 */
};

static const char * const SHIELD_EXPLOSION_SPRITE_2[] = {
    "90 30 3",
    "  0",
    "X 11",       /* Light Cyan */
    "O 9",        /* Bright Blue */
    "                              XX                       X   X  X         X   X             ", /* row  0 */
    "        X X        X     X X    X X   X X          X  X                       X           ", /* row  1 */
    "          X          X        X  X       X                         X          X           ", /* row  2 */
    "                   X X                  OX   X                X              X            ", /* row  3 */
    "     XX   X        X          X  OXO  X   OXO X XX    O  O                            X   ", /* row  4 */
    "           X             O   OOX X X    OOO O   X  OO XXXOXX  X    X   X  X               ", /* row  5 */
    "   X X           X   OX XO OXO OX  OXO  XO O O OO   OOO O  X   O O O                      ", /* row  6 */
    "          X       X   OX OX   OXOO  X    O O O  O  O OXX  OX  XO    XOOX         X      X ", /* row  7 */
    "    X           XOXO XX O      XO  OXO  X X  XXO   O   XOXOOXX  OO O X   X      X     XX  ", /* row  8 */
    "             X XX    XOO OO  XXO   XXX   O OX  X O XX      X     O      OXOX X            ", /* row  9 */
    "  X   X      XOX         O   OXXO  XX        OXOXXO  OXO  XXO  X OO  OX   OX              ", /* row 10 */
    "            OXX OX X O X  O  X O    O X            O XXO  X  XO   O X O    X      X X X   ", /* row 11 */
    "         X O OO  X O O X  OXO   OX X  O   O           O X X X XOXO O X   X   XX           ", /* row 12 */
    "X    X      O XXXX    XO     XO XX  O          O  O    OO   OO O X X  XXX OXOOXX          ", /* row 13 */
    "X          X  XO XO      O   X OO O O  OO         O      O  X   XO OXX    XOOX            ", /* row 14 */
    " X    X     O  X  O O XX X  OO X                     OOO  OO X               X            ", /* row 15 */
    "  X          O     X X  X OOOXX X       O  O            X   O    XX  X    X  X   X    X   ", /* row 16 */
    "       X    OO      XXXOOXO X   OX O      OO O O     O  XOX X    O       O   XO X    X    ", /* row 17 */
    "            O O XO  O   X     X XX X OO          O O    OX   X  O X  X   OO     X   X     ", /* row 18 */
    "      X X     XXO O  XOOXOX  O X X  XX  XOO  OO    XOOXO  OXXXO   O XXO OX                ", /* row 19 */
    "  X           O OOXO O O    OOXX    OXO   XO XX OX XXXO  XX OXX XOO  O     X           X  ", /* row 20 */
    "      X         X  XXOX X  O OOX OO    O O O   OOXOOXXX   OXO      XO  O X     X          ", /* row 21 */
    "   X   X X        X OXX   XOO    O OXO      XXXOX  X XO X   OXX X   XX                    ", /* row 22 */
    "  X               X       X XX O XOOXXXXXO  OOX   XO XO X X X X O XO                      ", /* row 23 */
    "                        XX          OO     OOX O      XO XXX O XX                   X     ", /* row 24 */
    "X X     X                   X X  OX X OOOOX  XX   X OXOXXX               X X      XX     X", /* row 25 */
    "                            X     X   X     X O  X   X  X        XX  X       X XX X       ", /* row 26 */
    "      X    X   X   X       X X                X     X                                     ", /* row 27 */
    "                    X   X    X                       X X   X                  XXX         ", /* row 28 */
    "                           X       X            XX    X  X              X                 ", /* row 29 */
};

static const char * const SHIELD_EXPLOSION_SPRITE_3[] = {
    "90 30 3",
    "  0",
    "X 11",       /* Light Cyan */
    "O 9",        /* Bright Blue */
    "      O O   X O X  O OX O    O       OX     X X   XX O XOX O    O OO         X     O     O", /* row  0 */
    " O   O                 OX  O       O   X  XX      O       OO   O        O                 ", /* row  1 */
    "       OOXOO O   X       O  O   X O   O     OO  OOXO   X  O O    OX   O    O        O     ", /* row  2 */
    "      O  XO  O    O X      XX  X O      OO  OX  O                X    OX     O           O", /* row  3 */
    "O     O   X     O O  O X  X     O X     O  X           O OO XO       OOOX  O      O       ", /* row  4 */
    "  X OX   OO       O          O  X      O   XO X OOOO  OO     O    XO    XO        O       ", /* row  5 */
    "O OXOO      O O    O            XX XOO   O     O    X     X               OO    O      X  ", /* row  6 */
    "O O         O X    OO      OO    O    X          X        O        O OO O  O XO  OOO  O   ", /* row  7 */
    "OOX      O OO O OO   O  O                                    OX           O  OO X  XO OO X", /* row  8 */
    "  O O O  O               X                                                 O   OO  X    O ", /* row  9 */
    "  X X  X X O      OO                               X                X  X         X    O O ", /* row 10 */
    "O  X O O  O O     OOO                          X  X   X          X   X X  O   O      OO   ", /* row 11 */
    "O     O X   X OOOO X                                   X             OO X   O OO    O    O", /* row 12 */
    "         O                                                        X   O  O    O OO     O  ", /* row 13 */
    "XX      O X OO  O X          X           X                 X          O     O     O  O    ", /* row 14 */
    " O      O  O  XOO  OX                            X        X            O O  O X    O   XOO", /* row 15 */
    "O  OO  X  OXO O OO O                             X                              O         ", /* row 16 */
    "OX      X  O   O O  O   X                X                               XO          X  O ", /* row 17 */
    "O     O  O O X OXO                                                               O      X ", /* row 18 */
    " XO             O                                              X                O         ", /* row 19 */
    " O  O X   O        X XX  X         X                                 O       X O       O  ", /* row 20 */
    "    OO       OX   X        O                                 OX   O OO O  O O     XO X    ", /* row 21 */
    "     X O   X O  O    X X     X  O                     X        OXOO    O    O X O OO X X O", /* row 22 */
    " O   O   OOX     O   O    OX  O   OX  XOX     X      O  OO                    O  O  O  O  ", /* row 23 */
    "  OOX  O     X  X   OX   X      OXX    O O     O O O X O   OO          O      O   X      O", /* row 24 */
    "O O           O         O      O     X                   O O X  O  O  O   O  O O          ", /* row 25 */
    "    X    X       O X     O     O   O  X   XO  O               OO X     O   O X O O     X O", /* row 26 */
    "      O     XO XX  XX O    O X      OX   OXO O  O      OOX O  O  X           OOOO O       ", /* row 27 */
    "XO    OO  O X      O      OOX        X         XXX  XO        O OO    O   O        X   OOX", /* row 28 */
    "  X O                  XOX  X    OO     OO           X O    O O    X          X   X  XO   ", /* row 29 */
};

static const char * const SHIELD_EXPLOSION_SPRITE_4[] = {
    "90 30 3",
    "  0",
    "X 11",       /* Light Cyan */
    "O 9",        /* Bright Blue */
    " X  O     O                                X             O OX   O                O        ", /* row  0 */
    "                          X          O       O         X     O                            ", /* row  1 */
    " X       O  X              O O       O                                   O        O       ", /* row  2 */
    "                                                     O                        O         O ", /* row  3 */
    "     O      X O            O           O            O                     X        O      ", /* row  4 */
    "                     O                                   X    XX               X          ", /* row  5 */
    "   X O  X    X               O           O    O     O                  O           O      ", /* row  6 */
    "                              XX       O                      O             O             ", /* row  7 */
    "   O                       X                                         O           O        ", /* row  8 */
    "                                                O                O                        ", /* row  9 */
    "                  X                    X  O           X          O                        ", /* row 10 */
    "           O       O    X        XX    O       OX         O              X  O O           ", /* row 11 */
    "                                    X                            X X     O                ", /* row 12 */
    "                                                                         O                ", /* row 13 */
    "              O    X X        XXO O  O                                               OO   ", /* row 14 */
    "               O         O           O                             O                      ", /* row 15 */
    "                X    O     O                                              O  O    O       ", /* row 16 */
    "                                              X         O       O  OO               XO    ", /* row 17 */
    "  X       O                 X                   O        X        O             O         ", /* row 18 */
    "            X    O   O                                                        O           ", /* row 19 */
    "    O         X O        O                    O                    O      O               ", /* row 20 */
    "   O         O                OO                  X                    O   O              ", /* row 21 */
    "          O     O            O     X                 O          X                      X  ", /* row 22 */
    "            O                                                X O               O          ", /* row 23 */
    "                    O      X X      O           X                                    OO   ", /* row 24 */
    "  X  O                                         O                                     O    ", /* row 25 */
    "           O            O                   X                                             ", /* row 26 */
    "                             O            O                                    O O        ", /* row 27 */
    "X                 X   X        X                                       O                  ", /* row 28 */
    "X                                          X  O         O                                 ", /* row 29 */
};

static const char * const * const SHIELD_EXPLOSION_SPRITES[] = {
    SHIELD_EXPLOSION_SPRITE_1,
    SHIELD_EXPLOSION_SPRITE_2,
    SHIELD_EXPLOSION_SPRITE_3,
    SHIELD_EXPLOSION_SPRITE_4
};

/* ---------------------------------------------------------------------
 * HEART_SPRITE  -  21 x 17 pixels, 2 colors
 * ------------------------------------------------------------------- */
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

#endif /* SPRITES_H */
