#include "scoreboard.h"
#include "graphics.h"
#include "font.h"

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
        FILE *created = fopen(SCOREBOARD_FILE, "w");
        if (created != NULL) fclose(created);
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
    if (sb == NULL || screen_w <= 0 || screen_h <= 0) return;

    /* ------------------------------------------------------------------ *
     * Visual constants                                                     *
     * ------------------------------------------------------------------ */

    /* 8-bit VGA palette indices */
    enum {
        COLOR_TITLE     = 0x0B, /* bright cyan   – title & decorations    */
        COLOR_HEADER    = 0x07, /* light gray    – column headers         */
        COLOR_GOLD      = 0x06, /* dark yellow   – 1st place (gold/amber) */
        COLOR_YELLOW    = 0x0E, /* bright yellow – 2nd place              */
        COLOR_WHITE     = 0x0F, /* bright white  – 3rd place              */
        COLOR_NORMAL    = 0x0F, /* bright white  – 4th place and below    */
        COLOR_SEPARATOR = 0x08, /* dark gray     – separator lines        */
        COLOR_BG        = 0x00  /* black         – background             */
    };

    /* Row padding (in pixels, unscaled) added between rows               */
    const int ROW_PAD    = 3;
    /* Top margin before the title                                         */
    const int TOP_MARGIN = 30;

    /* ------------------------------------------------------------------ *
     * Per-rank visual parameters                                           *
     * rank_scale[0] → 1st place, [1] → 2nd, [2] → 3rd, [3] → 4th+       *
     * ------------------------------------------------------------------ */
    static const uint8_t  rank_scale[4] = { 3, 2, 2, 2 };
    static const uint32_t rank_color[4] = {
        COLOR_GOLD, COLOR_YELLOW, COLOR_WHITE, COLOR_NORMAL
    };

    int cur_y = TOP_MARGIN;

    /* ------------------------------------------------------------------ *
     * 1.  Title  "SCOREBOARD"                                             *
     * ------------------------------------------------------------------ */
    {
        const char *title  = "SCOREBOARD";
        const int   sc     = 3;
        int tw = font_string_width(title, sc);
        int tx = (screen_w - tw) / 2;
        font_draw_string(tx, cur_y, title, COLOR_TITLE, sc);
        cur_y += FONT_GLYPH_H * sc + ROW_PAD * 2;
    }

    /* ------------------------------------------------------------------ *
     * 2.  Separator under the title                                        *
     * ------------------------------------------------------------------ */
    {
        const int SEP_MARGIN = 40;
        graphics_draw_rectangle(
            (uint16_t) SEP_MARGIN,
            (uint16_t) cur_y,
            (uint16_t)(screen_w - 2 * SEP_MARGIN),
            2,
            COLOR_SEPARATOR);
        cur_y += 2 + ROW_PAD * 2;
    }

    /* ------------------------------------------------------------------ *
     * 3.  Column headers                                                   *
     * ------------------------------------------------------------------ */
    {
        const char *hdr = "#   NAME             SCORE";
        const int   sc  = 1;
        int hw = font_string_width(hdr, sc);
        int hx = (screen_w - hw) / 2;
        font_draw_string(hx, cur_y, hdr, COLOR_HEADER, sc);
        cur_y += FONT_GLYPH_H * sc + ROW_PAD;

        /* thin separator under headers */
        graphics_draw_rectangle(
            (uint16_t)(screen_w / 4),
            (uint16_t) cur_y,
            (uint16_t)(screen_w / 2),
            1,
            COLOR_SEPARATOR);
        cur_y += 1 + ROW_PAD * 2;
    }

    /* ------------------------------------------------------------------ *
     * 4.  Scoreboard entries                                               *
     * ------------------------------------------------------------------ */
    int display_count = sb->count < SCOREBOARD_MAX_DISPLAY
                        ? sb->count
                        : SCOREBOARD_MAX_DISPLAY;

    for (int i = 0; i < display_count; i++) {
        /* Rank-based visual parameters (0-indexed, cap at index 3)        */
        int      param_idx = i < 3 ? i : 3;
        int      sc        = (int) rank_scale[param_idx];
        uint32_t col       = rank_color[param_idx];

        const ScoreEntry *e = &sb->entries[i];

        /* Build the entry string: "#N  NAME  SCORE"                       */
        char buf[64];
        snprintf(buf, sizeof(buf), "#%-2d %-20s %d", i + 1, e->name, e->score);

        /* Centre the text horizontally                                     */
        int tw = font_string_width(buf, sc);
        int tx = (screen_w - tw) / 2;

        font_draw_string(tx, cur_y, buf, col, sc);

        /* Advance y by this row's rendered height plus padding            */
        cur_y += FONT_GLYPH_H * sc + ROW_PAD * sc;

        /* Safety: stop if we'd go off screen                              */
        if (cur_y >= screen_h - FONT_GLYPH_H) break;
    }

    /* ------------------------------------------------------------------ *
     * 5.  Empty-list placeholder                                           *
     * ------------------------------------------------------------------ */
    if (display_count == 0) {
        const char *empty = "NO SCORES YET";
        const int   sc    = 2;
        int tw = font_string_width(empty, sc);
        int tx = (screen_w - tw) / 2;
        font_draw_string(tx, cur_y, empty, COLOR_NORMAL, sc);
    }

    /* ------------------------------------------------------------------ *
     * 6.  Footer                                                           *
     * ------------------------------------------------------------------ */
    {
        const char *footer = "PRESS SPACE TO RETURN TO MENU";
        const int   sc     = 2;
        int fw = font_string_width(footer, sc);
        int fx = (screen_w - fw) / 2;
        int fy = screen_h - FONT_GLYPH_H * sc - 20;
        font_draw_string(fx, fy, footer, COLOR_TITLE, sc);
    }
}
