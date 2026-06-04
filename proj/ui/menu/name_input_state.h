#ifndef NAME_INPUT_STATE_H
#define NAME_INPUT_STATE_H

#include "app.h"
#include <stdint.h>

void name_input_state_on_enter    (AppContext *ctx);
void name_input_state_update      (AppContext *ctx);
void name_input_state_draw        (const AppContext *ctx);
void name_input_state_handle_key  (AppContext *ctx, uint8_t sc);
void name_input_state_handle_mouse(AppContext *ctx, int x, int y, bool lb);

#endif /* NAME_INPUT_STATE_H */
