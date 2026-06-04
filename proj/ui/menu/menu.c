#include <stdint.h>
#include <string.h>

#include "menu.h"
#include "graphics.h"
#include "font.h"
#include "keyboard.h"

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

#define BTN_PLAY_Y   230
#define BTN_SCORES_Y (BTN_PLAY_Y + BTN_H + 15)
#define BTN_QUIT_Y   (BTN_SCORES_Y + BTN_H + 15)

/* Controls section: sits below the last button */
#define CTRL_Y  (BTN_QUIT_Y + BTN_H + 36)

/* Name-input box geometry */
#define NI_BOX_X    162
#define NI_BOX_Y    330
#define NI_BOX_W    700
#define NI_BOX_H     68
#define NI_BOX_BRD    2
#define NI_BOX_PAD   10

/* Cursor blink period (timer ticks) */
#define NI_BLINK_PERIOD 30

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

#define COL_CURSOR       0x0Fu
#define COL_CURSOR_DOT   0x0Cu

/* Name-input specific colours */
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
/* Fixed star field                                                    */
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
/* Internal drawing helpers                                            */
/* ------------------------------------------------------------------ */

static bool point_in_rect(int px, int py, int rx, int ry, int rw, int rh) {
  return px >= rx && px < rx + rw && py >= ry && py < ry + rh;
}

static void draw_background(void) {
  graphics_clear(COL_BG);

  for (int i = 0; i < STAR_COUNT; i++) {
    uint32_t col = (i % 3 == 0) ? COL_STAR_DIM : COL_STAR;
    graphics_draw_pixel(STARS[i][0], STARS[i][1], col);
    if (i % 7 == 0)
      graphics_draw_rectangle(STARS[i][0], STARS[i][1], 2, 2, COL_STAR);
  }

  for (int t = 0; t < 2; t++) {
    graphics_draw_hline(0, (uint16_t) t,                              (uint16_t)graphics_get_hres(), COL_BORDER);
    graphics_draw_hline(0, (uint16_t)(graphics_get_vres() - 1u - t), (uint16_t)graphics_get_hres(), COL_BORDER);
  }
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

static void draw_cursor(int cx, int cy) {
  int lx = cx - 10;
  int rx = cx + 4;
  if (lx < 0) lx = 0;
  if (rx + 6 >= (int)graphics_get_hres()) rx = (int)graphics_get_hres() - 7;
  graphics_draw_hline((uint16_t) lx, (uint16_t) cy,
                      (uint16_t)(cx - 3 - lx), COL_CURSOR);
  graphics_draw_hline((uint16_t)(cx + 4), (uint16_t) cy,
                      (uint16_t)(rx - cx - 3), COL_CURSOR);
  for (int dy = -10; dy <= 10; dy++) {
    if (dy >= -2 && dy <= 2) continue;
    int py = cy + dy;
    if (py >= 0 && py < (int)graphics_get_vres())
      graphics_draw_pixel((uint16_t) cx, (uint16_t) py, COL_CURSOR);
  }
  graphics_draw_rectangle((uint16_t)(cx - 1), (uint16_t)(cy - 1),
                           3, 3, COL_CURSOR_DOT);
}

/* ------------------------------------------------------------------ */
/* Main-screen draw                                                    */
/* ------------------------------------------------------------------ */

static void draw_main_screen(const Menu *m) {
  draw_background();

  /* Title box */
  graphics_draw_rectangle(TITLE_X,     TITLE_Y,     TITLE_W,     TITLE_H,     COL_BORDER);
  graphics_draw_rectangle(TITLE_X + 2, TITLE_Y + 2, TITLE_W - 4, TITLE_H - 4, COL_TITLE_BOX);
  {
    int w = font_string_width("STAR", 5);
    font_draw_string((int)graphics_get_hres() / 2 - w / 2, TITLE_Y + 14, "STAR", COL_STAR_WORD, 5);
  }
  {
    int w = font_string_width("SHOOTER", 4);
    font_draw_string((int)graphics_get_hres() / 2 - w / 2, TITLE_Y + 86, "SHOOTER", COL_SHOOT_WORD, 4);
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
    int hw = font_string_width("CONTROLS", 2);
    font_draw_string((int)graphics_get_hres() / 2 - hw / 2, cy, "CONTROLS", COL_CTRL_HDR, 2);
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
      int rw = font_string_width(ROWS[i], 2);
      font_draw_string((int)graphics_get_hres() / 2 - rw / 2, cy, ROWS[i], COL_CTRL_TEXT, 2);
      cy += FONT_GLYPH_H * 2 + 4;
    }
  }

  /* Bottom hint */
  {
    const char *hint = "HOVER A BUTTON AND LEFT-CLICK TO SELECT";
    int hw = font_string_width(hint, 1);
    font_draw_string((int)graphics_get_hres() / 2 - hw / 2, (int)graphics_get_vres() - 28, hint, COL_HINT, 1);
  }

  /* Mouse cursor — always on top */
  draw_cursor(m->mouse_x, m->mouse_y);
}

/* ------------------------------------------------------------------ */
/* Name-input screen draw                                              */
/* ------------------------------------------------------------------ */

static void draw_name_input_screen(const Menu *m) {
  draw_background();

  /* Title */
  {
    const char *title = "ENTER YOUR NAME";
    int tw = font_string_width(title, 3);
    font_draw_string(((int)graphics_get_hres() - tw) / 2, 120, title, COL_NI_BOX_BRD, 3);
  }

  /* Sub-hint above box */
  {
    const char *sub = "TYPE YOUR NAME AND PRESS ENTER";
    int sw = font_string_width(sub, 1);
    font_draw_string(((int)graphics_get_hres() - sw) / 2, 290, sub, COL_NI_HINT, 1);
  }

  /* Empty-name warning (shown only when attempted with blank field) */
  if (m->name_empty_warning) {
    const char *warn = "NAME CANNOT BE EMPTY";
    int ww = font_string_width(warn, 1);
    font_draw_string(((int)graphics_get_hres() - ww) / 2, NI_BOX_Y - 28, warn, COL_NI_WARNING, 1);
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

    font_draw_string(cx, text_y, m->player_name, COL_NI_TEXT, 3);

    /* Cursor block: placed after the typed text */
    int text_w = font_string_width(m->player_name, 3);
    int cur_x  = cx + (m->name_len > 0 ? text_w + FONT_SPACING * 3 : 0);

    if (m->name_blink_show) {
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
    const char *h1 = "PRESS ENTER TO PLAY";
    font_draw_string(((int)graphics_get_hres() - font_string_width(h1, 2)) / 2, hy, h1, COL_NI_HINT, 2);
    hy += FONT_GLYPH_H * 2 + 10;
    const char *h2 = "PRESS ESC TO CANCEL";
    font_draw_string(((int)graphics_get_hres() - font_string_width(h2, 1)) / 2, hy, h2, COL_NI_CANCEL, 1);
  }
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

void menu_init(Menu *m) {
  if (m == NULL) return;
  m->screen           = MENU_SCREEN_MAIN;
  m->mouse_x          = (int)graphics_get_hres() / 2;
  m->mouse_y          = (int)graphics_get_vres() / 2;
  m->play_hover       = false;
  m->scores_hover     = false;
  m->quit_hover       = false;
  m->lb_prev          = false;
  m->action           = MENU_ACTION_NONE;
  memset(m->player_name, 0, sizeof(m->player_name));
  m->name_len          = 0;
  m->name_blink_show   = true;
  m->name_blink_timer  = NI_BLINK_PERIOD;
  m->name_empty_warning = false;
}

void menu_update(Menu *m, int cursor_x, int cursor_y, bool lb) {
  if (m == NULL) return;

  /* Mouse interaction only applies on the main buttons screen */
  if (m->screen != MENU_SCREEN_MAIN) return;

  m->mouse_x = cursor_x;
  m->mouse_y = cursor_y;
  m->action  = MENU_ACTION_NONE;

  m->play_hover   = point_in_rect(cursor_x, cursor_y, BTN_X, BTN_PLAY_Y,   BTN_W, BTN_H);
  m->scores_hover = point_in_rect(cursor_x, cursor_y, BTN_X, BTN_SCORES_Y, BTN_W, BTN_H);
  m->quit_hover   = point_in_rect(cursor_x, cursor_y, BTN_X, BTN_QUIT_Y,   BTN_W, BTN_H);

  if (!lb && m->lb_prev) {
    if (m->play_hover) {
      /* Transition to name-input screen instead of firing MENU_ACTION_PLAY */
      m->screen            = MENU_SCREEN_NAME_INPUT;
      m->play_hover        = false;
      memset(m->player_name, 0, sizeof(m->player_name));
      m->name_len           = 0;
      m->name_blink_show    = true;
      m->name_blink_timer   = NI_BLINK_PERIOD;
      m->name_empty_warning = false;
    }
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
  if (m->screen == MENU_SCREEN_NAME_INPUT)
    draw_name_input_screen(m);
  else
    draw_main_screen(m);
}

void menu_name_input_handle_key(Menu *m, uint8_t scancode) {
  if (m == NULL || m->screen != MENU_SCREEN_NAME_INPUT) return;

  /* ESC break → cancel, back to main buttons */
  if (scancode == KBD_ESC_BREAK) {
    m->screen = MENU_SCREEN_MAIN;
    memset(m->player_name, 0, sizeof(m->player_name));
    m->name_len           = 0;
    m->name_empty_warning = false;
    return;
  }

  /* ENTER → confirm or warn */
  if (scancode == KBD_ENTER_MAKE) {
    if (m->name_len > 0) {
      m->player_name[m->name_len] = '\0';
      m->action = MENU_ACTION_PLAY;
    } else {
      m->name_empty_warning = true;
    }
    return;
  }

  /* BACKSPACE → delete last character */
  if (scancode == KBD_BACKSPACE_MAKE) {
    if (m->name_len > 0) {
      m->name_len--;
      m->player_name[m->name_len] = '\0';
      m->name_empty_warning = false;
    }
    return;
  }

  /* Letter keys → append */
  if (scancode < 128) {
    char ch = SC_TO_CHAR[scancode];
    if (ch != '\0' && m->name_len < SCOREBOARD_NAME_LEN - 1) {
      m->player_name[m->name_len++] = ch;
      m->player_name[m->name_len]   = '\0';
      m->name_empty_warning = false;
    }
  }
}

void menu_name_input_update(Menu *m) {
  if (m == NULL || m->screen != MENU_SCREEN_NAME_INPUT) return;
  m->name_blink_timer--;
  if (m->name_blink_timer <= 0) {
    m->name_blink_show  = !m->name_blink_show;
    m->name_blink_timer = NI_BLINK_PERIOD;
  }
}

bool menu_is_name_input_active(const Menu *m) {
  return m != NULL && m->screen == MENU_SCREEN_NAME_INPUT;
}

const char *menu_get_player_name(const Menu *m) {
  if (m == NULL) return "";
  return m->player_name;
}
