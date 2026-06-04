#ifndef NAME_INPUT_H
#define NAME_INPUT_H

#include "scoreboard.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  NAME_INPUT_CONTINUE,
  NAME_INPUT_CONFIRMED,
  NAME_INPUT_CANCELLED
} NameInputResult;

typedef struct {
  char name[SCOREBOARD_NAME_LEN];
  int  name_len;
  bool blink_show;
  int  blink_timer;
  bool empty_warning;
} NameInput;

void            name_input_init      (NameInput *ni);
NameInputResult name_input_handle_key(NameInput *ni, uint8_t scancode);
void            name_input_update    (NameInput *ni);
void            name_input_draw      (const NameInput *ni);

#endif /* NAME_INPUT_H */
