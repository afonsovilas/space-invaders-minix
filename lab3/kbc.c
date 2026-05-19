#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include <sys/types.h>

#ifndef __minix
typedef uint32_t u32_t;
#endif

#include "i8042.h"
#include "kbc.h"

static int kbd_hook_id = 1;

static volatile uint8_t ih_byte = 0;
static volatile bool ih_has_byte = false;

int (kbd_subscribe_int)(uint8_t *bit_no) {
	if (bit_no == NULL) return 1;

	*bit_no = (uint8_t) kbd_hook_id;

	if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != 0) return 1;

	return 0;
}

int (kbd_unsubscribe_int)(void) {
	if (sys_irqrmpolicy(&kbd_hook_id) != 0) return 1;
	return 0;
}

void (kbc_ih)(void) {
	uint8_t status;
	uint8_t data;

	if (util_sys_inb(KBC_STAT_REG, &status) != 0) return;

	if (!(status & KBC_ST_OBF)) return;

	if (util_sys_inb(KBC_OUT_BUF, &data) != 0) return;

	if (status & (KBC_ST_AUX | KBC_ST_ERR)) {
		ih_has_byte = false;
		return;
	}

	ih_byte = data;
	ih_has_byte = true;
}

bool (kbc_has_byte)(void) {
	return ih_has_byte;
}

uint8_t (kbc_get_byte)(void) {
	ih_has_byte = false;
	return ih_byte;
}

static int (kbc_wait_ibf_clear)(void) {
	uint8_t status;

	for (int i = 0; i < KBC_MAX_TRIES; i++) {
		if (util_sys_inb(KBC_STAT_REG, &status) != 0) return 1;
		if (!(status & KBC_ST_IBF)) return 0;
		tickdelay(micros_to_ticks(KBC_DELAY_US));
	}

	return 1;
}

int (kbc_read_byte)(uint8_t *byte) {
	if (byte == NULL) return 1;

	uint8_t status;
	uint8_t data;

	for (int i = 0; i < KBC_MAX_TRIES; i++) {
		if (util_sys_inb(KBC_STAT_REG, &status) != 0) return 1;

		if (status & KBC_ST_OBF) {
			if (util_sys_inb(KBC_OUT_BUF, &data) != 0) return 1;

			if (status & (KBC_ST_AUX | KBC_ST_ERR)) {
				tickdelay(micros_to_ticks(KBC_DELAY_US));
				continue;
			}

			*byte = data;
			return 0;
		}

		tickdelay(micros_to_ticks(KBC_DELAY_US));
	}

	return 1;
}

int (kbc_read_cmd_byte)(uint8_t *cmd_byte) {
	if (cmd_byte == NULL) return 1;

	if (kbc_wait_ibf_clear() != 0) return 1;
	if (sys_outb(KBC_CMD_REG, KBC_CMD_READ_CMD_BYTE) != 0) return 1;

	return kbc_read_byte(cmd_byte);
}

int (kbc_write_cmd_byte)(uint8_t cmd_byte) {
	if (kbc_wait_ibf_clear() != 0) return 1;
	if (sys_outb(KBC_CMD_REG, KBC_CMD_WRITE_CMD_BYTE) != 0) return 1;

	if (kbc_wait_ibf_clear() != 0) return 1;
	if (sys_outb(KBC_IN_BUF, cmd_byte) != 0) return 1;

	return 0;
}
