#include <string.h>

#include "name_input.h"
#include "menu.h"
#include "graphics.h"
#include "font.h"
#include "keyboard.h"

/* ------------------------------------------------------------------ */
/* Name-input box geometry                                             */
/* ------------------------------------------------------------------ */

#define NI_BOX_X    162
#define NI_BOX_Y    330
#define NI_BOX_W    700
#define NI_BOX_H     68
#define NI_BOX_BRD    2
#define NI_BOX_PAD   10

#define NI_BLINK_PERIOD 30

/* ------------------------------------------------------------------ */
/* Colour palette                                                      */
/* ------------------------------------------------------------------ */

#define COL_NI_BOX_BRD   0x0Bu  /* cyan border      */
#define COL_NI_BOX_FILL  0x01u  /* dark blue fill   */
#define COL_NI_TEXT      0x0Fu  /* white typed text */
#define COL_NI_CURSOR    0x0Bu  /* cyan cursor block*/
#define COL_NI_HINT      0x07u  /* light grey       */
#define COL_NI_CANCEL    0x08u  /* dark grey        */
#define COL_NI_WARNING   0x0Cu  /* bright red       */

/* ------------------------------------------------------------------ */
/* Scancode → uppercase letter (0x00–0x7F)                            */
/* '\0' means "not a letter key"                                       */
/* ------------------------------------------------------------------ */

static const char SC_TO_CHAR[128] = {
  /* 0x00 */ '\0',
  /* 0x01 */ '\0', /* ESC make      */
  /* 0x02 */ '\0', /* 1             */
  /* 0x03 */ '\0', /* 2             */
  /* 0x04 */ '\0', /* 3             */
  /* 0x05 */ '\0', /* 4             */
  /* 0x06 */ '\0', /* 5             */
  /* 0x07 */ '\0', /* 6             */
  /* 0x08 */ '\0', /* 7             */
  /* 0x09 */ '\0', /* 8             */
  /* 0x0A */ '\0', /* 9             */
  /* 0x0B */ '\0', /* 0             */
  /* 0x0C */ '\0', /* -             */
  /* 0x0D */ '\0', /* =             */
  /* 0x0E */ '\0', /* BACKSPACE     */
  /* 0x0F */ '\0', /* TAB           */
  /* 0x10 */ 'Q',
  /* 0x11 */ 'W',
  /* 0x12 */ 'E',
  /* 0x13 */ 'R',
  /* 0x14 */ 'T',
  /* 0x15 */ 'Y',
  /* 0x16 */ 'U',
  /* 0x17 */ 'I',
  /* 0x18 */ 'O',
  /* 0x19 */ 'P',
  /* 0x1A */ '\0', /* [             */
  /* 0x1B */ '\0', /* ]             */
  /* 0x1C */ '\0', /* ENTER         */
  /* 0x1D */ '\0', /* CTRL          */
  /* 0x1E */ 'A',
  /* 0x1F */ 'S',
  /* 0x20 */ 'D',
  /* 0x21 */ 'F',
  /* 0x22 */ 'G',
  /* 0x23 */ 'H',
  /* 0x24 */ 'J',
  /* 0x25 */ 'K',
  /* 0x26 */ 'L',
  /* 0x27 */ '\0', /* ;             */
  /* 0x28 */ '\0', /* '             */
  /* 0x29 */ '\0', /* `             */
  /* 0x2A */ '\0', /* LSHIFT        */
  /* 0x2B */ '\0', /* \             */
  /* 0x2C */ 'Z',
  /* 0x2D */ 'X',
  /* 0x2E */ 'C',
  /* 0x2F */ 'V',
  /* 0x30 */ 'B',
  /* 0x31 */ 'N',
  /* 0x32 */ 'M',
  /* 0x33–0x7F: all '\0' */
  '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0',
};

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

void name_input_init(NameInput *ni) {
  if (ni == NULL) return;
  memset(ni->name, 0, sizeof(ni->name));
  ni->name_len      = 0;
  ni->blink_show    = true;
  ni->blink_timer   = NI_BLINK_PERIOD;
  ni->empty_warning = false;
}

NameInputResult name_input_handle_key(NameInput *ni, uint8_t scancode) {
  if (ni == NULL) return NAME_INPUT_CONTINUE;

  if (scancode == KBD_ESC_BREAK) {
    return NAME_INPUT_CANCELLED;
  }

  if (scancode == KBD_ENTER_MAKE) {
    if (ni->name_len > 0) {
      ni->name[ni->name_len] = '\0';
      return NAME_INPUT_CONFIRMED;
    }
    ni->empty_warning = true;
    return NAME_INPUT_CONTINUE;
  }

  if (scancode == KBD_BACKSPACE_MAKE) {
    if (ni->name_len > 0) {
      ni->name_len--;
      ni->name[ni->name_len] = '\0';
      ni->empty_warning = false;
    }
    return NAME_INPUT_CONTINUE;
  }

  if (scancode < 128) {
    char ch = SC_TO_CHAR[scancode];
    if (ch != '\0' && ni->name_len < SCOREBOARD_NAME_LEN - 1) {
      ni->name[ni->name_len++] = ch;
      ni->name[ni->name_len]   = '\0';
      ni->empty_warning = false;
    }
  }

  return NAME_INPUT_CONTINUE;
}

void name_input_update(NameInput *ni) {
  if (ni == NULL) return;
  ni->blink_timer--;
  if (ni->blink_timer <= 0) {
    ni->blink_show  = !ni->blink_show;
    ni->blink_timer = NI_BLINK_PERIOD;
  }
}

void name_input_draw(const NameInput *ni) {
  if (ni == NULL) return;

  menu_draw_background();

  /* Title */
  font_draw_string_centred((int)graphics_get_hres() / 2, 120,
                           "ENTER YOUR NAME", COL_NI_BOX_BRD, 3);

  /* Sub-hint above box */
  font_draw_string_centred((int)graphics_get_hres() / 2, 290,
                           "TYPE YOUR NAME AND PRESS ENTER", COL_NI_HINT, 1);

  /* Empty-name warning */
  if (ni->empty_warning) {
    font_draw_string_centred((int)graphics_get_hres() / 2, NI_BOX_Y - 28,
                             "NAME CANNOT BE EMPTY", COL_NI_WARNING, 1);
  }

  /* Input box — outer border then inner fill */
  graphics_draw_rectangle(NI_BOX_X, NI_BOX_Y, NI_BOX_W, NI_BOX_H, COL_NI_BOX_BRD);
  graphics_draw_rectangle(NI_BOX_X + NI_BOX_BRD, NI_BOX_Y + NI_BOX_BRD,
                           NI_BOX_W - 2 * NI_BOX_BRD, NI_BOX_H - 2 * NI_BOX_BRD,
                           COL_NI_BOX_FILL);

  /* Typed text + blinking cursor inside box */
  {
    int text_y = NI_BOX_Y + NI_BOX_H / 2 - (FONT_GLYPH_H * 3) / 2;
    int cx     = NI_BOX_X + NI_BOX_BRD + NI_BOX_PAD;

    font_draw_string(cx, text_y, ni->name, COL_NI_TEXT, 3);

    int text_w = font_string_width(ni->name, 3);
    int cur_x  = cx + (ni->name_len > 0 ? text_w + FONT_SPACING * 3 : 0);

    if (ni->blink_show) {
      graphics_draw_rectangle((uint16_t) cur_x,
                               (uint16_t) text_y,
                               (uint16_t)(FONT_GLYPH_W * 3),
                               (uint16_t)(FONT_GLYPH_H * 3),
                               COL_NI_CURSOR);
    }
  }

  /* Hints below box */
  {
    int hy = NI_BOX_Y + NI_BOX_H + 28;
    font_draw_string_centred((int)graphics_get_hres() / 2, hy,
                             "PRESS ENTER TO PLAY", COL_NI_HINT, 2);
    hy += FONT_GLYPH_H * 2 + 10;
    font_draw_string_centred((int)graphics_get_hres() / 2, hy,
                             "PRESS ESC TO CANCEL", COL_NI_CANCEL, 1);
  }
}
