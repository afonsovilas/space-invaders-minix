#include <string.h>

#include "name_input.h"
#include "menu.h"
#include "graphics.h"
#include "font.h"
#include "background.h"

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

  draw_starfield_background();

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
