#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

/**
 * @defgroup utils Driver Utilities
 * Shared low-level utility functions used across hardware drivers.
 * @{
 */

/**
 * Read one byte from an I/O port via sys_inb.
 * LCF wraps this function for grading (hence the parentheses around the name).
 *
 * @param port  I/O port address.
 * @param value Output pointer for the byte read.
 * @return 0 on success, non-zero on failure.
 */
int (util_sys_inb)(int port, uint8_t *value);

/**@}*/

#endif /* UTILS_H */
