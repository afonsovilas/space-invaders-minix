#include <lcom/lcf.h>
#include <stdint.h>

#include "utils.h"

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t data;
  if (value == NULL) return 1;
  if (sys_inb(port, &data) != 0) return 1;
  *value = (uint8_t)(data & 0xFF);
  return 0;
}
