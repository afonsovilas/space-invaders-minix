#ifndef NAME_INPUT_H
#define NAME_INPUT_H

#include "scoreboard.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup name_input Name Input Widget
 * UI widget for capturing and displaying player name input with cursor blinking
 * and empty-name validation warning.
 * @{
 */

/**
 * @struct NameInput
 * Stores the state of the name input widget.
 */
typedef struct {
  char name[SCOREBOARD_NAME_LEN]; /**< Typed name buffer */
  int  name_len;                  /**< Current length of typed name */
  bool blink_show;                /**< Whether cursor should be visible this frame */
  int  blink_timer;               /**< Countdown timer for cursor blink animation */
  bool empty_warning;             /**< Flag to display "name cannot be empty" warning */
} NameInput;

/**
 * Initializes a NameInput widget.
 * Clears the name buffer and resets animation state.
 *
 * @param ni Pointer to the NameInput structure to initialize
 */
void name_input_init(NameInput *ni);

/**
 * Updates the NameInput widget (cursor blinking animation).
 * Must be called once per frame. Decrements the blink timer and toggles cursor visibility.
 *
 * @param ni Pointer to the NameInput structure to update
 */
void name_input_update(NameInput *ni);

/**
 * Renders the NameInput widget to the back-buffer.
 * Draws the input box, typed text, blinking cursor, and any validation warnings.
 *
 * @param ni Pointer to the NameInput structure to render
 */
void name_input_draw(const NameInput *ni);

/** @} */

#endif /* NAME_INPUT_H */
