#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 */

/* IRQ */

#define KBD_IRQ 1

/* Portas I/O */

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

/* Comandos */

#define KBC_CMD_READ_CMD_BYTE  0x20
#define KBC_CMD_WRITE_CMD_BYTE 0x60

#define KBC_CMD_CHECK_KBD_IF   0xAB
#define KBC_CMD_DISABLE_KBD_IF 0xAD
#define KBC_CMD_ENABLE_KBD_IF  0xAE

/* Command Byte */

#define KBC_CB_INT BIT(0)

/* Utilitários */

#define KBC_DELAY_US   20000
#define KBC_MAX_TRIES  10

#define KBD_ESC_BREAK  0x81

/**@}*/

#endif /* _LCOM_I8042_H_ */
