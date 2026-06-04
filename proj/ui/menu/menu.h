#ifndef MENU_H
#define MENU_H

#include "scoreboard.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  MENU_ACTION_NONE,
  MENU_ACTION_PLAY,
  MENU_ACTION_SCORES,
  MENU_ACTION_QUIT
} MenuAction;

typedef struct {
  int  mouse_x, mouse_y;
  bool play_hover, scores_hover, quit_hover;
  bool lb_prev;
  MenuAction action;
} Menu;

void       menu_init          (Menu *m);
void       menu_update        (Menu *m, int cursor_x, int cursor_y, bool lb);
MenuAction menu_get_action    (const Menu *m);
void       menu_draw          (const Menu *m);
void       menu_draw_background(void);

#endif /* MENU_H */
