#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <stdint.h>
#include <time.h>

/**
 * Maximum number of entries displayed on the scoreboard screen.
 * The log file itself is unbounded; this cap only applies to rendering.
 */
#define SCOREBOARD_MAX_DISPLAY 10

/** Maximum length of a player name, including the null terminator. */
#define SCOREBOARD_NAME_LEN 32

/** Absolute path to the append-only CSV log file used for persistent score storage. */
#define SCOREBOARD_FILE "/scoreboard.csv"

/**
 * @brief Represents a single scoreboard entry.
 *
 * Stores the player name, their final score, and the Unix timestamp of when
 * the entry was recorded.
 */
typedef struct {
    char name[SCOREBOARD_NAME_LEN]; /**< Player name (null-terminated). */
    int  score;                     /**< Final score achieved by the player. */
    long timestamp;                 /**< Unix timestamp of the entry. */
} ScoreEntry;

/**
 * @brief Holds all scoreboard entries loaded into memory.
 *
 * @c entries points to a heap-allocated array whose size matches the exact
 * number of lines in the log file at load time. The array is always kept
 * sorted by score in descending order. The caller is responsible for releasing
 * this memory via @ref scoreboard_free when the scoreboard is no longer needed.
 */
typedef struct {
    ScoreEntry *entries; /**< Heap-allocated array of score entries, sorted descending by score. */
    int         count;   /**< Number of valid entries in @c entries. */
} Scoreboard;

/**
 * @brief Loads all entries from the log file into memory.
 *
 * Opens @ref SCOREBOARD_FILE for reading. A first pass counts the number of
 * lines to determine the exact allocation size; a second pass parses each
 * line as a comma-separated record: @c name,score,timestamp. After loading,
 * the entries are sorted in descending order by score.
 *
 * If the file does not exist the scoreboard is initialised as empty
 * (@c entries = NULL, @c count = 0) and the function still returns success.
 *
 * @note The caller must not call this function on a @c Scoreboard that already
 *       holds a live allocation without first calling @ref scoreboard_free, as
 *       this would leak memory.
 *
 * @param sb Pointer to the Scoreboard structure to populate. Must not be NULL.
 * @return 0 on success, -1 if @p sb is NULL or memory allocation fails.
 */
int scoreboard_load(Scoreboard *sb);

/**
 * @brief Appends a new entry to the log file and updates the in-memory list.
 *
 * Creates a new @ref ScoreEntry with the given @p name, @p score, and the
 * current time obtained via @c time(NULL). The entry is written as a single
 * line to @ref SCOREBOARD_FILE using @c fopen("a"), preserving all previous
 * entries (the file is never overwritten). The in-memory array is grown by
 * one slot via @c realloc and re-sorted in descending order by score.
 *
 * @param sb    Pointer to the in-memory Scoreboard to update. Must not be NULL.
 * @param name  Null-terminated player name. Truncated to @ref SCOREBOARD_NAME_LEN - 1 characters.
 * @param score Final score to record.
 * @return 0 on success, -1 if @p sb or @p name is NULL, the file could not be
 *         opened, or memory reallocation fails.
 */
int scoreboard_add(Scoreboard *sb, const char *name, int score);

/**
 * @brief Releases the heap memory owned by a Scoreboard.
 *
 * Frees the @c entries array allocated by @ref scoreboard_load or grown by
 * @ref scoreboard_add, then resets @c entries to NULL and @c count to 0.
 * Safe to call on an already-freed or never-loaded Scoreboard.
 *
 * Should be called once on game shutdown to avoid memory leaks.
 *
 * @param sb Pointer to the Scoreboard to free. Passing NULL is a no-op.
 */
void scoreboard_free(Scoreboard *sb);

/**
 * @brief Renders the scoreboard on the VBE graphics screen.
 *
 * Displays a formatted table of the top @ref SCOREBOARD_MAX_DISPLAY entries
 * in @p sb, centred on the screen. Each row shows the rank, player name,
 * score, and date derived from the stored Unix timestamp.
 *
 * @note This function depends on @c graphics_draw_string, which requires the
 *       bitmap font renderer to be implemented in @c graphics.c. The body is
 *       currently a stub and will be filled in when text rendering is added.
 *
 * @param sb       Pointer to the Scoreboard to display. Must not be NULL.
 * @param screen_w Width of the VBE screen in pixels.
 * @param screen_h Height of the VBE screen in pixels.
 */
void scoreboard_draw(const Scoreboard *sb, int screen_w, int screen_h);

#endif /* SCOREBOARD_H */
