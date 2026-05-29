#include "scoreboard.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * @brief Comparator for qsort: sorts ScoreEntry elements by score descending.
 *
 * Passed directly to @c qsort when sorting the entries array. A negative
 * return value means @p a should come before @p b (i.e. @p a has a higher
 * score).
 *
 * @param a Pointer to the first ScoreEntry (as @c void *).
 * @param b Pointer to the second ScoreEntry (as @c void *).
 * @return Negative if @p a has a higher score, positive if @p b does, 0 if equal.
 */
static int score_compare(const void *a, const void *b) {
    const ScoreEntry *ea = (const ScoreEntry *) a;
    const ScoreEntry *eb = (const ScoreEntry *) b;
    /* Descending order: subtract a from b so that higher scores sort first. */
    return eb->score - ea->score;
}

/**
 * @brief Counts the number of newline characters in an open file.
 *
 * Reads the file character by character and counts occurrences of @c '\\n'.
 * The file position is rewound to the beginning before returning so the
 * caller can continue reading from the start.
 *
 * @param fp Open file pointer. Must not be NULL.
 * @return Number of newline characters found, which equals the number of
 *         complete lines for well-formed CSV files.
 */
static int count_lines(FILE *fp) {
    if (fp == NULL) {
        return -1;
    }

    int lines = 0;
    int c;

    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            lines++;
        }
    }

    rewind(fp);
    return lines;
}

int scoreboard_load(Scoreboard *sb) {
    if (sb == NULL) {
        return -1;
    }

    sb->entries = NULL;
    sb->count   = 0;

    FILE *fp = fopen(SCOREBOARD_FILE, "r");
    if (fp == NULL) {
        return 0;
    }

    int line_count = count_lines(fp);
    if (line_count == 0) {
        fclose(fp);
        return 0;
    }

    sb->entries = (ScoreEntry *) malloc((size_t) line_count * sizeof(ScoreEntry));
    if (sb->entries == NULL) {
        fclose(fp);
        return -1;
    }

    while (sb->count < line_count) {
        ScoreEntry *entry = &sb->entries[sb->count];

        int matched = fscanf(fp,
                             " %31[^,],%d,%ld",
                             entry->name,
                             &entry->score,
                             &entry->timestamp);

        if (matched != 3) {
            break;
        }

        entry->name[SCOREBOARD_NAME_LEN - 1] = '\0';
        sb->count++;
    }

    fclose(fp);

    qsort(sb->entries, (size_t) sb->count, sizeof(ScoreEntry), score_compare);

    return 0;
}

int scoreboard_add(Scoreboard *sb, const char *name, int score) {
    if (sb == NULL || name == NULL) {
        return -1;
    }

    /* Append a single line to the immutable log – never overwrite. */
    FILE *fp = fopen(SCOREBOARD_FILE, "a");
    if (fp == NULL) {
        return -1;
    }

    long ts = (long) time(NULL);
    fprintf(fp, "%.*s,%d,%ld\n", SCOREBOARD_NAME_LEN - 1, name, score, ts);
    fclose(fp);

    ScoreEntry *resized = (ScoreEntry *) realloc(sb->entries,
                                                 (size_t) (sb->count + 1) * sizeof(ScoreEntry));
    if (resized == NULL) {
        return -1;
    }

    sb->entries = resized;

    ScoreEntry *entry = &sb->entries[sb->count];
    strncpy(entry->name, name, SCOREBOARD_NAME_LEN - 1);
    entry->name[SCOREBOARD_NAME_LEN - 1] = '\0';
    entry->score     = score;
    entry->timestamp = ts;
    sb->count++;

    /* Re-sort so the new entry lands in its correct rank position. */
    qsort(sb->entries, (size_t) sb->count, sizeof(ScoreEntry), score_compare);

    return 0;
}

void scoreboard_free(Scoreboard *sb) {
    if (sb == NULL) {
        return;
    }

    free(sb->entries);
    sb->entries = NULL;
    sb->count   = 0;
}

void scoreboard_draw(const Scoreboard *sb, int screen_w, int screen_h) {
    /*
     * TODO: Implement once graphics_draw_string is available in graphics.c.
     *
     * Planned layout (centred on screen):
     *
     *   SCOREBOARD
     *   ──────────────────────────────────────
     *   #   NAME             SCORE   DATE
     *   1   Alice            1500    2026-05-29
     *   2   Bob               900    2026-05-28
     *   ...                        (top SCOREBOARD_MAX_DISPLAY entries)
     *
     * Each row rendered with graphics_draw_string at scale 2.
     * Timestamp formatted with strftime(..., "%Y-%m-%d", localtime(...)).
     */
    (void) sb;
    (void) screen_w;
    (void) screen_h;
}
