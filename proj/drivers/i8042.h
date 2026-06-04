#ifndef _PROJ_I8042_H_
#define _PROJ_I8042_H_

#include <lcom/lcf.h>
#include "utils.h"

/** @defgroup i8042 i8042
 * Constants for the i8042 KBC and PS/2 mouse/keyboard controller.
 * @{
 */

/* IRQ lines */
#define KBD_IRQ   1
#define MOUSE_IRQ 12

/* I/O ports */
#define KBC_OUT_BUF  0x60  /**< KBC output buffer (read)  */
#define KBC_IN_BUF   0x60  /**< KBC input  buffer (write) */
#define KBC_STAT_REG 0x64  /**< KBC status register (read)  */
#define KBC_CMD_REG  0x64  /**< KBC command register (write) */

/* Status Register bits */
#define KBC_ST_OBF     BIT(0)  /**< Output buffer full */
#define KBC_ST_IBF     BIT(1)  /**< Input  buffer full */
#define KBC_ST_SYS     BIT(2)
#define KBC_ST_A2      BIT(3)
#define KBC_ST_INH     BIT(4)
#define KBC_ST_AUX     BIT(5)  /**< Mouse data in output buffer */
#define KBC_ST_TIMEOUT BIT(6)
#define KBC_ST_PARITY  BIT(7)

#define KBC_ST_ERR (KBC_ST_PARITY | KBC_ST_TIMEOUT)

/* KBC Commands */
#define KBC_CMD_READ_CMD_BYTE  0x20
#define KBC_CMD_WRITE_CMD_BYTE 0x60
#define KBC_CMD_DISABLE_AUX    0xA7
#define KBC_CMD_ENABLE_AUX     0xA8
#define KBC_CMD_WRITE_MOUSE    0xD4

/* KBC Command Byte bits */
#define KBC_CB_INT_KBD   BIT(0)
#define KBC_CB_INT_MOUSE BIT(1)

/* Mouse Commands */
#define MOUSE_CMD_SET_STREAM_MODE     0xEA
#define MOUSE_CMD_SET_REMOTE_MODE     0xF0
#define MOUSE_CMD_READ_DATA           0xEB
#define MOUSE_CMD_ENABLE_DATA_REPORT  0xF4
#define MOUSE_CMD_DISABLE_DATA_REPORT 0xF5

/* Mouse Response Codes */
#define MOUSE_ACK   0xFA
#define MOUSE_NACK  0xFE
#define MOUSE_ERROR 0xFC

/* Mouse Packet Byte 0 bits */
#define MOUSE_LB       BIT(0)  /**< Left   button pressed */
#define MOUSE_RB       BIT(1)  /**< Right  button pressed */
#define MOUSE_MB       BIT(2)  /**< Middle button pressed */
#define MOUSE_SYNC_BIT BIT(3)  /**< Always 1 in first packet byte */
#define MOUSE_X_SIGN   BIT(4)  /**< X movement sign */
#define MOUSE_Y_SIGN   BIT(5)  /**< Y movement sign */
#define MOUSE_X_OV     BIT(6)  /**< X overflow */
#define MOUSE_Y_OV     BIT(7)  /**< Y overflow */

/* Timing */
#define KBC_DELAY_US  20000
#define KBC_MAX_TRIES 10

/**@}*/

#endif /* _PROJ_I8042_H_ */
