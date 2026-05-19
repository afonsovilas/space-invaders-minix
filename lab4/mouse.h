#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include <stdbool.h>
#include <stdint.h>

int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)(void);

void (mouse_ih)(void);

bool (mouse_has_byte)(void);
uint8_t (mouse_get_byte)(void);

int (mouse_send_cmd)(uint8_t cmd);
int (mouse_send_cmd_arg)(uint8_t cmd, uint8_t arg);

int (mouse_disable_data_reporting)(void);
int (mouse_enable_data_reporting_local)(void);
int (mouse_set_stream_mode)(void);
int (mouse_set_remote_mode)(void);

#endif /* _LCOM_MOUSE_H_ */
