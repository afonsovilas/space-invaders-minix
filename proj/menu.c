#include <stdint.h>

#include "menu.h"
#include "graphics.h"
#include "font.h"

/* ------------------------------------------------------------------ */
/* Screen & layout constants                                           */
/* ------------------------------------------------------------------ */

#define SCREEN_W 1024
#define SCREEN_H  768

/* Title box */
#define TITLE_X  162
#define TITLE_Y   30
#define TITLE_W  700
#define TITLE_H  155

/* Buttons */
#define BTN_X     322
#define BTN_W     380
#define BTN_H      68
#define BTN_BRD     2   /* border thickness */

#define BTN_PLAY_Y  230
#define BTN_QUIT_Y  325

/* Controls section */
#define CTRL_Y  (BTN_QUIT_Y + BTN_H + 36)

/* ------------------------------------------------------------------ */
/* Colour palette (VGA 8-bit indexed)                                  */
/* 0x00=Black  0x01=DkBlue  0x02=DkGreen  0x04=DkRed                 */
/* 0x07=LtGrey 0x08=DkGrey  0x09=Blue     0x0A=Green                 */
/* 0x0B=Cyan   0x0C=Red     0x0E=Yellow   0x0F=White                 */
/* ------------------------------------------------------------------ */

#define COL_BG          0x00u
#define COL_STAR        0x0Fu
#define COL_STAR_DIM    0x07u
#define COL_BORDER      0x0Bu  /* cyan frame */
#define COL_TITLE_BOX   0x01u  /* dark blue  */
#define COL_STAR_WORD   0x0Eu  /* yellow     */
#define COL_SHOOT_WORD  0x0Bu  /* cyan       */
#define COL_SEP         0x08u  /* dark grey  */

#define COL_BTN_NORM_OUT 0x09u
#define COL_BTN_NORM_IN  0x01u
#define COL_BTN_PLAY_OUT 0x0Au
#define COL_BTN_PLAY_IN  0x02u
#define COL_BTN_QUIT_OUT 0x0Cu
#define COL_BTN_QUIT_IN  0x04u
#define COL_BTN_TEXT     0x0Fu

#define COL_CTRL_HDR    0x0Bu
#define COL_CTRL_TEXT   0x07u
#define COL_HINT        0x08u

#define COL_CURSOR      0x0Fu
#define COL_CURSOR_DOT  0x0Cu  /* red centre dot */

/* ------------------------------------------------------------------ */
/* Fixed star field (50 points spread across the screen)              */
/* ------------------------------------------------------------------ */

static const uint16_t STARS[][2] = {
  { 18, 48}, {140, 28}, {290, 66}, {492, 12}, {693, 42}, {898, 22}, {978, 77},
  { 48,148}, {248,128}, {390,162}, {648,138}, {848,168}, {998,152},
  { 98,248}, {348,228}, {598,258}, {798,243}, {948,276},
  { 28,348}, {178,368}, {448,338}, {748,362}, {922,340},
  { 78,448}, {318,468}, {598,452}, {900,476}, {150,490},
  {198,548}, {498,558}, {798,543}, {910,560},
  { 98,648}, {698,658}, {350,640}, {850,635},
  { 55,700}, {400,715}, {700,720}, {950,705},
  {250, 10}, {600,  8}, {800,  5}, {450,740}, {150,750},
  {550,380}, {820,400}, {75,400}, {200,680}, {700,190},
};
#define STAR_COUNT ((int)(sizeof(STARS) / sizeof(STARS[0])))

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

static bool point_in_rect(int px, int py, int rx, int ry, int rw, int rh) {
  return px >= rx && px < rx + rw && py >= ry && py < ry + rh;
}

/** Draw a button with a 2-px border; render the label centred inside. */
static void draw_button(int x, int y, int w, int h, const char *label,
                        bool hover, uint32_t col_outer, uint32_t col_inner) {
  /* Outer fill (becomes the border) */
  graphics_draw_rectangle((uint16_t) x, (uint16_t) y,
                           (uint16_t) w, (uint16_t) h, col_outer);
  /* Inner fill */
  graphics_draw_rectangle((uint16_t)(x + BTN_BRD), (uint16_t)(y + BTN_BRD),
                           (uint16_t)(w - 2 * BTN_BRD), (uint16_t)(h - 2 * BTN_BRD),
                           col_inner);

  /* Subtle highlight stripe on hover */
  if (hover) {
    graphics_draw_hline((uint16_t)(x + BTN_BRD + 2),
                        (uint16_t)(y + BTN_BRD),
                        (uint16_t)(w - 2 * BTN_BRD - 4),
                        0x07u);
  }

  /* Centred label at scale 3 */
  int tw = font_string_width(label, 3);
  int tx = x + w / 2 - tw / 2;
  int ty = y + h / 2 - (FONT_GLYPH_H * 3) / 2;
  font_draw_string(tx, ty, label, COL_BTN_TEXT, 3);
}

/** Draw the crosshair cursor. Always called last so it appears on top. */
static void draw_cursor(int cx, int cy) {
  /* Horizontal arms (gap of 3 px each side of centre) */
  int lx = cx - 10;
  int rx = cx + 4;
  if (lx < 0) lx = 0;
  if (rx + 6 >= SCREEN_W) rx = SCREEN_W - 7;
  graphics_draw_hline((uint16_t) lx, (uint16_t) cy, (uint16_t)(cx - 3 - lx), COL_CURSOR);
  graphics_draw_hline((uint16_t)(cx + 4), (uint16_t) cy, (uint16_t)(rx - cx - 3), COL_CURSOR);

  /* Vertical arms */
  for (int dy = -10; dy <= 10; dy++) {
    if (dy >= -2 && dy <= 2) continue;  /* gap */
    int py = cy + dy;
    if (py >= 0 && py < SCREEN_H)
      graphics_draw_pixel((uint16_t) cx, (uint16_t) py, COL_CURSOR);
  }

  /* Centre dot 3×3 */
  graphics_draw_rectangle((uint16_t)(cx - 1), (uint16_t)(cy - 1), 3, 3, COL_CURSOR_DOT);
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

void menu_init(Menu *m) {
  if (m == NULL) return;
  m->mouse_x   = SCREEN_W / 2;
  m->mouse_y   = SCREEN_H / 2;
  m->play_hover = false;
  m->quit_hover = false;
  m->lb_prev    = false;
  m->action     = MENU_ACTION_NONE;
}

void menu_update(Menu *m, int cursor_x, int cursor_y, bool lb) {
  if (m == NULL) return;

  m->mouse_x = cursor_x;
  m->mouse_y = cursor_y;
  m->action  = MENU_ACTION_NONE;

  m->play_hover = point_in_rect(cursor_x, cursor_y, BTN_X, BTN_PLAY_Y, BTN_W, BTN_H);
  m->quit_hover = point_in_rect(cursor_x, cursor_y, BTN_X, BTN_QUIT_Y, BTN_W, BTN_H);

  /* Trigger on rising edge of left button (click = press + release) */
  if (!lb && m->lb_prev) {
    if (m->play_hover) m->action = MENU_ACTION_PLAY;
    if (m->quit_hover) m->action = MENU_ACTION_QUIT;
  }

  m->lb_prev = lb;
}

MenuAction menu_get_action(const Menu *m) {
  if (m == NULL) return MENU_ACTION_NONE;
  return m->action;
}

void menu_draw(const Menu *m) {
  if (m == NULL) return;

  /* ---- Background ---- */
  graphics_clear(COL_BG);

  /* ---- Star field ---- */
  for (int i = 0; i < STAR_COUNT; i++) {
    uint32_t col = (i % 3 == 0) ? COL_STAR_DIM : COL_STAR;
    graphics_draw_pixel(STARS[i][0], STARS[i][1], col);
    /* Occasional 2×2 bright star */
    if (i % 7 == 0)
      graphics_draw_rectangle(STARS[i][0], STARS[i][1], 2, 2, COL_STAR);
  }

  /* ---- Screen border (2 px thick, cyan) ---- */
  for (int t = 0; t < 2; t++) {
    graphics_draw_hline(0,          (uint16_t) t,              SCREEN_W, COL_BORDER);
    graphics_draw_hline(0,          (uint16_t)(SCREEN_H-1-t),  SCREEN_W, COL_BORDER);
  }

  /* ---- Title box ---- */
  /* Outer cyan border */
  graphics_draw_rectangle(TITLE_X,     TITLE_Y,     TITLE_W,     TITLE_H,     COL_BORDER);
  /* Inner dark-blue fill */
  graphics_draw_rectangle(TITLE_X + 2, TITLE_Y + 2, TITLE_W - 4, TITLE_H - 4, COL_TITLE_BOX);

  /* "STAR" – yellow, scale 5 */
  {
    int w = font_string_width("STAR", 5);
    font_draw_string(SCREEN_W / 2 - w / 2, TITLE_Y + 14, "STAR", COL_STAR_WORD, 5);
  }
  /* "SHOOTER" – cyan, scale 4 */
  {
    int w = font_string_width("SHOOTER", 4);
    font_draw_string(SCREEN_W / 2 - w / 2, TITLE_Y + 86, "SHOOTER", COL_SHOOT_WORD, 4);
  }

  /* ---- Separator under title ---- */
  graphics_draw_hline((uint16_t) TITLE_X, (uint16_t)(TITLE_Y + TITLE_H + 4),
                      (uint16_t) TITLE_W, COL_SEP);
  graphics_draw_hline((uint16_t) TITLE_X, (uint16_t)(TITLE_Y + TITLE_H + 5),
                      (uint16_t) TITLE_W, COL_SEP);

  /* ---- PLAY button ---- */
  {
    uint32_t out = m->play_hover ? COL_BTN_PLAY_OUT : COL_BTN_NORM_OUT;
    uint32_t in  = m->play_hover ? COL_BTN_PLAY_IN  : COL_BTN_NORM_IN;
    draw_button(BTN_X, BTN_PLAY_Y, BTN_W, BTN_H, "PLAY", m->play_hover, out, in);
    /* Arrow indicator on hover */
    if (m->play_hover)
      font_draw_string(BTN_X + BTN_W + 10,
                       BTN_PLAY_Y + BTN_H / 2 - FONT_GLYPH_H,
                       ">", COL_BTN_PLAY_OUT, 2);
  }

  /* ---- QUIT button ---- */
  {
    uint32_t out = m->quit_hover ? COL_BTN_QUIT_OUT : COL_BTN_NORM_OUT;
    uint32_t in  = m->quit_hover ? COL_BTN_QUIT_IN  : COL_BTN_NORM_IN;
    draw_button(BTN_X, BTN_QUIT_Y, BTN_W, BTN_H, "QUIT", m->quit_hover, out, in);
    if (m->quit_hover)
      font_draw_string(BTN_X + BTN_W + 10,
                       BTN_QUIT_Y + BTN_H / 2 - FONT_GLYPH_H,
                       ">", COL_BTN_QUIT_OUT, 2);
  }

  /* ---- Controls section ---- */
  {
    int cy = CTRL_Y;

    /* Header */
    int hw = font_string_width("CONTROLS", 2);
    font_draw_string(SCREEN_W / 2 - hw / 2, cy, "CONTROLS", COL_CTRL_HDR, 2);
    cy += FONT_GLYPH_H * 2 + 6;

    /* Separator */
    graphics_draw_hline(210, (uint16_t) cy, SCREEN_W - 420, COL_SEP);
    cy += 6;

    /* Key-action pairs */
    static const char * const ROWS[] = {
      "A / D   -   MOVE PLAYER",
      "SPACE   -   FIRE BULLET",
      "MOUSE   -   AIM + FIRE",
      "ESC     -   QUIT GAME",
    };
    for (int i = 0; i < 4; i++) {
      int rw = font_string_width(ROWS[i], 2);
      font_draw_string(SCREEN_W / 2 - rw / 2, cy, ROWS[i], COL_CTRL_TEXT, 2);
      cy += FONT_GLYPH_H * 2 + 4;
    }
  }

  /* ---- Bottom hint ---- */
  {
    const char *hint = "HOVER A BUTTON AND LEFT-CLICK TO SELECT";
    int hw = font_string_width(hint, 1);
    font_draw_string(SCREEN_W / 2 - hw / 2, SCREEN_H - 28, hint, COL_HINT, 1);
  }

  /* ---- Mouse cursor (always last — on top of everything) ---- */
  draw_cursor(m->mouse_x, m->mouse_y);
}
