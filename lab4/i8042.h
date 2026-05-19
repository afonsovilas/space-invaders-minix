#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 */

/* IRQ */

#define KBD_IRQ 1
#define MOUSE_IRQ 12

/* I/O ports */

#define KBC_OUT_BUF 0x60
#define KBC_IN_BUF  0x60

#define KBC_STAT_REG 0x64
#define KBC_CMD_REG  0x64

/* Status Register */

#define KBC_ST_OBF     BIT(0)
#define KBC_ST_IBF     BIT(1)
#define KBC_ST_SYS     BIT(2)
#define KBC_ST_A2      BIT(3)
#define KBC_ST_INH     BIT(4)
#define KBC_ST_AUX     BIT(5)
#define KBC_ST_TIMEOUT BIT(6)
#define KBC_ST_PARITY  BIT(7)

#define KBC_ST_ERR (KBC_ST_PARITY | KBC_ST_TIMEOUT)

/* KBC Commands */

#define KBC_CMD_READ_CMD_BYTE  0x20
#define KBC_CMD_WRITE_CMD_BYTE 0x60

#define KBC_CMD_CHECK_KBD_IF   0xAB
#define KBC_CMD_DISABLE_KBD_IF 0xAD
#define KBC_CMD_ENABLE_KBD_IF  0xAE

#define KBC_CMD_DISABLE_AUX    0xA7
#define KBC_CMD_ENABLE_AUX     0xA8
#define KBC_CMD_WRITE_MOUSE    0xD4

/* Command Byte */

#define KBC_CB_INT_KBD   BIT(0)
#define KBC_CB_INT_MOUSE BIT(1)

/* Mouse Commands */

#define MOUSE_CMD_SET_STREAM_MODE     0xEA
#define MOUSE_CMD_SET_REMOTE_MODE     0xF0
#define MOUSE_CMD_READ_DATA           0xEB
#define MOUSE_CMD_ENABLE_DATA_REPORT  0xF4
#define MOUSE_CMD_DISABLE_DATA_REPORT 0xF5

/* Mouse Responses */

#define MOUSE_ACK   0xFA
#define MOUSE_NACK  0xFE
#define MOUSE_ERROR 0xFC

/* Packet Bits */

#define MOUSE_LB       BIT(0)
#define MOUSE_RB       BIT(1)
#define MOUSE_MB       BIT(2)
#define MOUSE_SYNC_BIT BIT(3)
#define MOUSE_X_SIGN   BIT(4)
#define MOUSE_Y_SIGN   BIT(5)
#define MOUSE_X_OV     BIT(6)
#define MOUSE_Y_OV     BIT(7)

/* Utilities */

#define KBC_DELAY_US  20000
#define KBC_MAX_TRIES 10

/**@}*/

#endif /* _LCOM_I8042_H_ */
