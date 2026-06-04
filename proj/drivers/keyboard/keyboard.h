#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

/* ---- Special keys ---- */
#define KBD_ESC_BREAK       0x81
#define KBD_BACKSPACE_MAKE  0x0E
#define KBD_ENTER_MAKE      0x1C
#define KBD_SPACE_MAKE      0x39
#define KBD_SPACE_BREAK     0xB9

/* ---- Letter make-codes (top row) ---- */
#define KBD_Q_MAKE  0x10
#define KBD_W_MAKE  0x11
#define KBD_E_MAKE  0x12
#define KBD_R_MAKE  0x13
#define KBD_T_MAKE  0x14
#define KBD_Y_MAKE  0x15
#define KBD_U_MAKE  0x16
#define KBD_I_MAKE  0x17
#define KBD_O_MAKE  0x18
#define KBD_P_MAKE  0x19

/* ---- Letter make-codes (home row) ---- */
#define KBD_A_MAKE  0x1E
#define KBD_A_BREAK 0x9E
#define KBD_S_MAKE  0x1F
#define KBD_D_MAKE  0x20
#define KBD_D_BREAK 0xA0
#define KBD_F_MAKE  0x21
#define KBD_G_MAKE  0x22
#define KBD_H_MAKE  0x23
#define KBD_J_MAKE  0x24
#define KBD_K_MAKE  0x25
#define KBD_L_MAKE  0x26

/* ---- Letter make-codes (bottom row) ---- */
#define KBD_Z_MAKE  0x2C
#define KBD_X_MAKE  0x2D
#define KBD_C_MAKE  0x2E
#define KBD_V_MAKE  0x2F
#define KBD_B_MAKE  0x30
#define KBD_N_MAKE  0x31
#define KBD_M_MAKE  0x32

/* ---- Scancode → uppercase letter (make-codes 0x00–0x7F)  ---- */
/* '\0' means the key does not map to a letter.                   */
extern const char kbd_scancode_to_char[128];

int keyboard_subscribe_int(uint8_t *bit_no);
int keyboard_unsubscribe_int(void);
void keyboard_ih(void);

bool keyboard_has_data(void);
uint8_t keyboard_get_byte(void);
void keyboard_clear_data(void);

#endif