#ifndef __PU_SD3078_H
#define __PU_SD3078_H
#include <stdint.h>
#include "gd32f30x_libopt.h"

typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t week;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} sd3078_rtc_time_t;

void sd3078_gpio_init(void);
void sd3078_write_data(uint8_t addr, uint8_t length, uint8_t *data);
void sd3078_read_data(uint8_t addr, uint8_t length, uint8_t *data);

void sd3078_write_enable(void);
void sd3078_write_disable(void);

void sd3078_set_time(uint8_t *time);
void sd3078_get_time(uint8_t *time);

uint8_t sd3078_get_status(void);

#endif
