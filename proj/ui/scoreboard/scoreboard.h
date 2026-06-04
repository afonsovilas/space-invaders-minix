#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <stdint.h>
#include <time.h>

/**
 * @defgroup scoreboard Scoreboard
 * High scores management and display.
 * @{
 */

/** Maximum number of scores to display on screen. */
#define SCOREBOARD_MAX_DISPLAY 10

/** Maximum length of a player name (including null terminator). */
#define SCOREBOARD_NAME_LEN 32

/** Path to the score log file. */
#define SCOREBOARD_FILE "/scoreboard.csv"

/**
 * @struct ScoreEntry
 * A single score record.
 */
typedef struct {
  char name[SCOREBOARD_NAME_LEN]; /**< Player name */
  int  score;                     /**< Score value */
  long timestamp;                 /**< When score was recorded */
} ScoreEntry;

/**
 * @struct Scoreboard
 * Container for all loaded scores.
 * Keeps entries sorted by score (highest first).
 */
typedef struct {
  ScoreEntry *entries; /**< Array of scores */
  int count;           /**< Number of scores */
} Scoreboard;

/**
 * Loads all scores from the log file into memory.
 * Entries are sorted by score in descending order.
 * If file doesn't exist, scoreboard is initialized empty (no error).
 *
 * @param sb Pointer to Scoreboard to load into
 * @return 0 on success, -1 on failure
 */
int scoreboard_load(Scoreboard *sb);

/**
 * Adds a new score to the log file and updates the in-memory list.
 * Automatically re-sorts scores and updates the file.
 *
 * @param sb    Pointer to Scoreboard to update
 * @param name  Player name
 * @param score Score value
 * @return 0 on success, -1 on failure
 */
int scoreboard_add(Scoreboard *sb, const char *name, int score);

/**
 * Frees all memory used by the scoreboard.
 * Safe to call multiple times or on empty scoreboards.
 *
 * @param sb Pointer to Scoreboard to free
 */
void scoreboard_free(Scoreboard *sb);

/**
 * Renders the scoreboard on screen.
 * Displays the top scores in a formatted table.
 *
 * @param sb       Pointer to Scoreboard to display
 * @param screen_w Screen width in pixels
 * @param screen_h Screen height in pixels
 */
void scoreboard_draw(const Scoreboard *sb, int screen_w, int screen_h);

/** @} */

#endif /* SCOREBOARD_H */
