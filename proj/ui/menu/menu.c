#include <stdint.h>

#include "menu.h"
#include "graphics.h"
#include "font.h"
#include "background.h"

/* ------------------------------------------------------------------ */
/* Title box                                                           */
/* ------------------------------------------------------------------ */

#define TITLE_X  162
#define TITLE_Y   30
#define TITLE_W  700
#define TITLE_H  155

/* ------------------------------------------------------------------ */
/* Buttons                                                             */
/* ------------------------------------------------------------------ */

#define BTN_X     322
#define BTN_W     380
#define BTN_H      68
#define BTN_BRD     2   /* border thickness */

#define BTN_PLAY_Y   230
#define BTN_SCORES_Y (BTN_PLAY_Y + BTN_H + 15)
#define BTN_QUIT_Y   (BTN_SCORES_Y + BTN_H + 15)

/* Controls section: sits below the last button */
#define CTRL_Y  (BTN_QUIT_Y + BTN_H + 36)

/* ------------------------------------------------------------------ */
/* Colour palette (VGA 8-bit indexed)                                  */
/* ------------------------------------------------------------------ */

#define COL_BG           0x00u
#define COL_STAR         0x0Fu
#define COL_STAR_DIM     0x07u
#define COL_BORDER       0x0Bu  /* cyan frame       */
#define COL_TITLE_BOX    0x01u  /* dark blue        */
#define COL_STAR_WORD    0x0Eu  /* yellow           */
#define COL_SHOOT_WORD   0x0Bu  /* cyan             */
#define COL_SEP          0x08u  /* dark grey        */

#define COL_BTN_NORM_OUT  0x09u
#define COL_BTN_NORM_IN   0x01u
#define COL_BTN_PLAY_OUT  0x0Au
#define COL_BTN_PLAY_IN   0x02u
#define COL_BTN_SCORES_OUT 0x0Bu
#define COL_BTN_SCORES_IN  0x03u
#define COL_BTN_QUIT_OUT  0x0Cu
#define COL_BTN_QUIT_IN   0x04u
#define COL_BTN_TEXT      0x0Fu

#define COL_CTRL_HDR     0x0Bu
#define COL_CTRL_TEXT    0x07u
#define COL_HINT         0x08u

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

static bool point_in_rect(int px, int py, int rx, int ry, int rw, int rh) {
  return px >= rx && px < rx + rw && py >= ry && py < ry + rh;
}

static void draw_button(int x, int y, int w, int h, const char *label,
                        bool hover, uint32_t col_outer, uint32_t col_inner) {
  graphics_draw_rectangle((uint16_t) x, (uint16_t) y,
                           (uint16_t) w, (uint16_t) h, col_outer);
  graphics_draw_rectangle((uint16_t)(x + BTN_BRD), (uint16_t)(y + BTN_BRD),
                           (uint16_t)(w - 2 * BTN_BRD), (uint16_t)(h - 2 * BTN_BRD),
                           col_inner);
  if (hover) {
    graphics_draw_hline((uint16_t)(x + BTN_BRD + 2),
                        (uint16_t)(y + BTN_BRD),
                        (uint16_t)(w - 2 * BTN_BRD - 4), 0x07u);
  }
  int tw = font_string_width(label, 3);
  int tx = x + w / 2 - tw / 2;
  int ty = y + h / 2 - (FONT_GLYPH_H * 3) / 2;
  font_draw_string(tx, ty, label, COL_BTN_TEXT, 3);
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

void menu_init(Menu *m) {
  if (m == NULL) return;
  m->mouse_x      = (int)graphics_get_hres() / 2;
  m->mouse_y      = (int)graphics_get_vres() / 2;
  m->play_hover   = false;
  m->scores_hover = false;
  m->quit_hover   = false;
  m->lb_prev      = false;
  m->action       = MENU_ACTION_NONE;
}

void menu_update(Menu *m, int cursor_x, int cursor_y, bool lb) {
  if (m == NULL) return;

  m->mouse_x = cursor_x;
  m->mouse_y = cursor_y;
  m->action  = MENU_ACTION_NONE;

  m->play_hover   = point_in_rect(cursor_x, cursor_y, BTN_X, BTN_PLAY_Y,   BTN_W, BTN_H);
  m->scores_hover = point_in_rect(cursor_x, cursor_y, BTN_X, BTN_SCORES_Y, BTN_W, BTN_H);
  m->quit_hover   = point_in_rect(cursor_x, cursor_y, BTN_X, BTN_QUIT_Y,   BTN_W, BTN_H);

  if (!lb && m->lb_prev) {
    if (m->play_hover)   m->action = MENU_ACTION_PLAY;
    if (m->scores_hover) m->action = MENU_ACTION_SCORES;
    if (m->quit_hover)   m->action = MENU_ACTION_QUIT;
  }

  m->lb_prev = lb;
}

MenuAction menu_get_action(const Menu *m) {
  if (m == NULL) return MENU_ACTION_NONE;
  return m->action;
}

void menu_draw(const Menu *m) {
  if (m == NULL) return;

  draw_starfield_background();

  /* Title box */
  graphics_draw_rectangle(TITLE_X,     TITLE_Y,     TITLE_W,     TITLE_H,     COL_BORDER);
  graphics_draw_rectangle(TITLE_X + 2, TITLE_Y + 2, TITLE_W - 4, TITLE_H - 4, COL_TITLE_BOX);
  {
    font_draw_string_centred((int)graphics_get_hres() / 2, TITLE_Y + 14, "STAR", COL_STAR_WORD, 5);
    font_draw_string_centred((int)graphics_get_hres() / 2, TITLE_Y + 86, "SHOOTER", COL_SHOOT_WORD, 4);
  }

  /* Separator */
  graphics_draw_hline((uint16_t) TITLE_X, (uint16_t)(TITLE_Y + TITLE_H + 4),
                      (uint16_t) TITLE_W, COL_SEP);
  graphics_draw_hline((uint16_t) TITLE_X, (uint16_t)(TITLE_Y + TITLE_H + 5),
                      (uint16_t) TITLE_W, COL_SEP);

  /* PLAY button */
  {
    uint32_t out = m->play_hover ? COL_BTN_PLAY_OUT : COL_BTN_NORM_OUT;
    uint32_t in  = m->play_hover ? COL_BTN_PLAY_IN  : COL_BTN_NORM_IN;
    draw_button(BTN_X, BTN_PLAY_Y, BTN_W, BTN_H, "PLAY", m->play_hover, out, in);
    if (m->play_hover)
      font_draw_string(BTN_X + BTN_W + 10,
                       BTN_PLAY_Y + BTN_H / 2 - FONT_GLYPH_H,
                       ">", COL_BTN_PLAY_OUT, 2);
  }

  /* SCORES button */
  {
    uint32_t out = m->scores_hover ? COL_BTN_SCORES_OUT : COL_BTN_NORM_OUT;
    uint32_t in  = m->scores_hover ? COL_BTN_SCORES_IN  : COL_BTN_NORM_IN;
    draw_button(BTN_X, BTN_SCORES_Y, BTN_W, BTN_H, "SCORES", m->scores_hover, out, in);
    if (m->scores_hover)
      font_draw_string(BTN_X + BTN_W + 10,
                       BTN_SCORES_Y + BTN_H / 2 - FONT_GLYPH_H,
                       ">", COL_BTN_SCORES_OUT, 2);
  }

  /* QUIT button */
  {
    uint32_t out = m->quit_hover ? COL_BTN_QUIT_OUT : COL_BTN_NORM_OUT;
    uint32_t in  = m->quit_hover ? COL_BTN_QUIT_IN  : COL_BTN_NORM_IN;
    draw_button(BTN_X, BTN_QUIT_Y, BTN_W, BTN_H, "QUIT", m->quit_hover, out, in);
    if (m->quit_hover)
      font_draw_string(BTN_X + BTN_W + 10,
                       BTN_QUIT_Y + BTN_H / 2 - FONT_GLYPH_H,
                       ">", COL_BTN_QUIT_OUT, 2);
  }

  /* Controls section */
  {
    int cy = CTRL_Y;
    font_draw_string_centred((int)graphics_get_hres() / 2, cy, "CONTROLS", COL_CTRL_HDR, 2);
    cy += FONT_GLYPH_H * 2 + 6;
    graphics_draw_hline(210, (uint16_t) cy, (int)graphics_get_hres() - 420, COL_SEP);
    cy += 6;
    static const char * const ROWS[] = {
      "A / D   -   MOVE PLAYER",
      "SPACE   -   FIRE BULLET",
      "MOUSE   -   AIM + FIRE",
      "ESC     -   QUIT GAME",
    };
    for (int i = 0; i < 4; i++) {
      font_draw_string_centred((int)graphics_get_hres() / 2, cy, ROWS[i], COL_CTRL_TEXT, 2);
      cy += FONT_GLYPH_H * 2 + 4;
    }
  }

  /* Bottom hint */
  {
    const char *hint = "HOVER A BUTTON AND LEFT-CLICK TO SELECT";
    font_draw_string_centred((int)graphics_get_hres() / 2,
                             (int)graphics_get_vres() - 28, hint, COL_HINT, 1);
  }

  /* Mouse cursor — always on top */
  graphics_draw_crosshair(m->mouse_x, m->mouse_y);
}
