#ifndef I2C_H
#define I2C_H

#include <stdbool.h>
#include "gd32f30x.h"

typedef enum {
    I2C_START = 0,
    I2C_SEND_ADDRESS,
    I2C_CLEAR_ADDRESS_FLAG,
    I2C_TRANSMIT_DATA,
    I2C_STOP,
} I2C_STATUS;

#define I2C_OK (1)
#define I2C_FAIL (0)
#define I2C_END (1)

#define I2C0_SPEED (400000)
#define I2C0_LOCAL_ADDRESS (0x22)
#define I2C0_SLAVE_ADDRESS (0x10)
#define I2C_PAGE_SIZE (8)

#define I2CX (I2C0)
#define I2C_SCL_PIN (GPIO_PIN_6)
#define I2C_SDA_PIN (GPIO_PIN_7)
#define I2C_SCL_PORT (GPIOB)
#define I2C_SDA_PORT (GPIOB)

#define I2C_TIME_OUT (uint16_t)(10000)

void i2c0_init(void);
uint8_t i2c_buffer_read(uint8_t *p_buffer, uint8_t read_address, uint16_t number_of_byte);
uint8_t i2c_buffer_write(uint8_t *p_buffer, uint8_t write_address, uint16_t number_of_byte);

void i2c_interrupt_init(void);
//void i2c_interrupt_deinit(void);
//void i2c_master_receive(uint8_t *pBuf, uint32_t nBytes);
//void i2c_master_send(uint32_t id, uint8_t *pBuf, uint32_t nBytes);
void i2c_master_write_reg(uint8_t devAddr, uint8_t regAddr, uint8_t *pData, uint32_t length);
void i2c_master_read_reg(uint8_t devAddr, uint8_t regAddr, uint8_t *pBuf, uint32_t length);

#endif