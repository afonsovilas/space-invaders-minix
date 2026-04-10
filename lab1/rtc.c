#include "rtc.h"
#include <minix/syslib.h>

#define TODO return -1

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_REG_DAY 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_UIP_MSK (1 << 7)
#define RTC_DM_MSK (1 << 2)

static int bcd_to_bin(uint8_t bcd) { return ((bcd >> 4) * 10) + (bcd & 0x0F); }

int rtc_read_date(rtc_date *date) { 
    uint32_t reg32; 
    uint8_t regB;
    
    do {
        sys_outb(RTC_ADDR_REG, RTC_REG_A);
        sys_inb(RTC_DATA_REG, &reg32);
    } while ((uint8_t)reg32 & RTC_UIP_MSK);

    sys_outb(RTC_ADDR_REG, RTC_REG_DAY);
    sys_inb(RTC_DATA_REG, &reg32);
    date->day = (uint8_t)reg32;

    sys_outb(RTC_ADDR_REG, RTC_REG_MONTH);
    sys_inb(RTC_DATA_REG, &reg32);
    date->month = (uint8_t)reg32;

    sys_outb(RTC_ADDR_REG, RTC_REG_YEAR);
    sys_inb(RTC_DATA_REG, &reg32);
    date->year = (uint8_t)reg32;

    sys_outb(RTC_ADDR_REG, RTC_REG_B);
    sys_inb(RTC_DATA_REG, &reg32);
    regB = (uint8_t)reg32;

    if (!(regB & RTC_DM_MSK)) {
        date->day = bcd_to_bin(date->day);
        date->month = bcd_to_bin(date->month);
        date->year = bcd_to_bin(date->year);
    }

    return 0;
 }
