#include "app.h"
#include "game_state.h"
#include "menu_state.h"
#include "name_input_state.h"
#include "scoreboard_state.h"

const StateHandler STATES[4] = {
  [APP_STATE_MENU] = {
    menu_state_on_enter,
    menu_state_update,
    menu_state_draw,
    menu_state_handle_key,
    menu_state_handle_mouse,
  },
  [APP_STATE_NAME_INPUT] = {
    name_input_state_on_enter,
    name_input_state_update,
    name_input_state_draw,
    name_input_state_handle_key,
    name_input_state_handle_mouse,
  },
  [APP_STATE_PLAYING] = {
    game_state_on_enter,
    game_state_update,
    game_state_draw,
    game_state_handle_key,
    game_state_handle_mouse,
  },
  [APP_STATE_SCOREBOARD] = {
    scoreboard_state_on_enter,
    scoreboard_state_update,
    scoreboard_state_draw,
    scoreboard_state_handle_key,
    scoreboard_state_handle_mouse,
  },
};
