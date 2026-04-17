#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <stdbool.h>
#include <stdint.h>

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)(void);

void (kbc_ih)(void);

bool (kbc_has_byte)(void);
uint8_t (kbc_get_byte)(void);

int (kbc_read_byte)(uint8_t *byte);

int (kbc_read_cmd_byte)(uint8_t *cmd_byte);
int (kbc_write_cmd_byte)(uint8_t cmd_byte);

#endif /* _LCOM_KBC_H_ */
