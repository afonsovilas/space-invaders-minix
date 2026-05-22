#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

#define KBD_ESC_BREAK 0x81

#define KBD_A_MAKE 0x1E
#define KBD_A_BREAK 0x9E

#define KBD_D_MAKE 0x20
#define KBD_D_BREAK 0xA0

#define KBD_SPACE_MAKE 0x39
#define KBD_SPACE_BREAK 0xB9

int keyboard_subscribe_int(uint8_t *bit_no);
int keyboard_unsubscribe_int(void);
void keyboard_ih(void);

bool keyboard_has_data(void);
uint8_t keyboard_get_byte(void);
void keyboard_clear_data(void);

#endif