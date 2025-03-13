#include <stdio.h>
#include "i2c.h"
#include "gd32f30x.h"

#define I2C_SINGLE_BYTE (1)
#define I2C_DOUBLE_BYTE (2)
#define I2C_MULTIPLE_BYTE (3)

#define I2C_ERROR_TIMEOUT (1)
#define I2C_ERROR_BUS_BUSY (2)
#define I2C_SUCCESS (0)

uint16_t slave_address = I2C0_SLAVE_ADDRESS;

void i2c0_init(void)
{
    /*Enable necessary peripherals*/
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* connect PB6 to I2C0_SCL */
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    /* connect PB7 to I2C0_SDA */
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    gpio_pin_remap_config(GPIO_I2C0_REMAP, DISABLE);

    /* configure I2C0 address */
    rcu_periph_clock_enable(RCU_I2C0);
    i2c_clock_config(I2C0, I2C0_SPEED, I2C_DTCY_2);
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_LOCAL_ADDRESS);
    i2c_enable(I2C0);
    (void)I2C_STAT0(I2C0);
    (void)I2C_STAT1(I2C0);
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

static void i2c_bus_reset(void)
{
    i2c_deinit(I2C0);
    i2c_disable(I2C0);
    delay_1ms(5);

    GPIO_BC(I2C_SCL_PORT) = I2C_SCL_PIN;
    GPIO_BC(I2C_SDA_PORT) = I2C_SDA_PIN;
    gpio_init(I2C_SCL_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_init(I2C_SDA_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);

    for (int i = 0; i < 9; i++) {
        GPIO_BOP(I2C_SCL_PORT) = I2C_SCL_PIN;
        delay_1us(5);
        GPIO_BC(I2C_SCL_PORT) = I2C_SCL_PIN;
        delay_1us(5);
    }

    GPIO_BOP(I2C_SCL_PORT) = I2C_SCL_PIN;
    GPIO_BOP(I2C_SDA_PORT) = I2C_SDA_PIN;

    gpio_init(I2C_SCL_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_init(I2C_SDA_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);

    rcu_periph_reset_enable(RCU_I2C0RST);
    rcu_periph_reset_disable(RCU_I2C0RST);

    rcu_periph_clock_enable(RCU_I2C0);
    i2c_clock_config(I2C0, I2C0_SPEED, I2C_DTCY_2);
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_LOCAL_ADDRESS);
    i2c_enable(I2C0);
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

static uint8_t i2c_wait_standby_state_timeout(void)
{
    uint8_t state = I2C_START;
    uint8_t i2c_timeout_flag = 0;
    uint16_t timeout = 0;

    while (!(i2c_timeout_flag)) {
        switch (state) {
        case I2C_START: {
            while (i2c_flag_get(I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                i2c_start_on_bus(I2CX);
                timeout = 0;
                state = I2C_SEND_ADDRESS;
            } else {
                i2c_bus_reset();
                timeout = 0;
                state = I2C_START;
                printf("i2c bus is busy in EEPROM standby!\n");
            }
        }
        break;

        case I2C_SEND_ADDRESS: {
            while ((!i2c_flag_get(I2CX, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                i2c_master_addressing(I2CX, slave_address, I2C_TRANSMITTER);
                timeout = 0;
                state = I2C_CLEAR_ADDRESS_FLAG;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends start signal timeout in EEPROM standby!\n");
            }
        }
        break;

        case I2C_CLEAR_ADDRESS_FLAG: {
            while ((!((i2c_flag_get(I2CX, I2C_FLAG_ADDSEND)) || (i2c_flag_get(I2CX, I2C_FLAG_AERR)))) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                if (i2c_flag_get(I2CX, I2C_FLAG_ADDSEND)) {
                    i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
                    timeout = 0;
                    i2c_stop_on_bus(I2CX);
                    i2c_timeout_flag = I2C_OK;
                    return I2C_END;
                } else {
                    i2c_flag_clear(I2CX, I2C_FLAG_AERR);
                    timeout = 0;
                    state = I2C_STOP;
                }
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master clears address flag timeout in EEPROM standby!\n");
            }
        }
        break;

        case I2C_STOP: {
            i2c_stop_on_bus(I2CX);
            while ((I2C_CTL0(I2CX) & I2C_CTL0_STOP) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_START;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends stop signal timeout in EEPROM standby!\n");
            }
        }
        break;
        default: {
            state = I2C_START;
            timeout = 0;
            printf("i2c master sends start signal end in EEPROM standby!.\n");
        }
        break;
        }
    }
    return I2C_END;
}

static uint8_t i2c_page_write_timeout(uint8_t *p_buffer, uint8_t write_address, uint8_t number_of_byte)
{
    uint8_t state = I2C_START;
    uint32_t i2c_timeout_flag = 0;
    uint16_t timeout = 0;

    i2c_ack_config(I2CX, I2C_ACK_ENABLE);

    while (!(i2c_timeout_flag)) {
        switch (state) {
        case I2C_START: {
            while (i2c_flag_get(I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                i2c_start_on_bus(I2CX);
                timeout = 0;
                state = I2C_SEND_ADDRESS;
            } else {
                i2c_bus_reset();
                timeout = 0;
                state = I2C_START;
                printf("i2c bus is busy in WRITE!\n");
                return I2C_FAIL;
            }
        }
        break;

        case I2C_SEND_ADDRESS: {
            while ((!i2c_flag_get(I2CX, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                i2c_master_addressing(I2CX, slave_address, I2C_TRANSMITTER);
                timeout = 0;
                state = I2C_CLEAR_ADDRESS_FLAG;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends start signal timeout in WRITE!\n");
            }
        }
        break;

        case I2C_CLEAR_ADDRESS_FLAG: {
            while ((!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
                timeout = 0;
                state = I2C_TRANSMIT_DATA;
            } else {
                timeout = 0;
                state = I2C_START;
                i2c_bus_reset();
                printf("i2c master clears address flag timeout in WRITE!\n");
                return I2C_FAIL;
            }
        }
        break;

        case I2C_TRANSMIT_DATA: {
            while ((!i2c_flag_get(I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                i2c_data_transmit(I2CX, write_address);
                timeout = 0;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends EEPROM's internal address timeout in WRITE!\n");
            }
            while ((!i2c_flag_get(I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                timeout = 0;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends data timeout in WRITE!\n");
            }
            while (number_of_byte--) {
                i2c_data_transmit(I2CX, *p_buffer);
                p_buffer++;
                while ((!i2c_flag_get(I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if (timeout < I2C_TIME_OUT) {
                    timeout = 0;
                } else {
                    timeout = 0;
                    state = I2C_START;
                    printf("i2c master sends data timeout in WRITE!\n");
                }
            }
            timeout = 0;
            state = I2C_STOP;
        }
        break;

        case I2C_STOP: {
            i2c_stop_on_bus(I2CX);
            while ((I2C_CTL0(I2CX) & I2C_CTL0_STOP) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_END;
                i2c_timeout_flag = I2C_OK;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends stop signal timeout in WRITE!\n");
            }
        }
        break;

        default: {
            state = I2C_START;
            i2c_timeout_flag = I2C_OK;
            timeout = 0;
            printf("i2c master sends start signal in WRITE.\n");
        }
        break;
        }
    }
    return I2C_END;
}

uint8_t i2c_buffer_read(uint8_t *p_buffer, uint8_t read_address, uint16_t number_of_byte)
{
    uint8_t state = I2C_START;
    uint8_t read_cycle = 0;
    uint8_t i2c_timeout_flag = 0;
    uint32_t timeout = 0;

    i2c_ack_config(I2CX, I2C_ACK_ENABLE);

    while (!(i2c_timeout_flag)) {
        switch (state) {

        case I2C_START: {
            if (RESET == read_cycle) {
                while (i2c_flag_get(I2CX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if (timeout < I2C_TIME_OUT) {
                    if (2 == number_of_byte) {
                        i2c_ackpos_config(I2CX, I2C_ACKPOS_NEXT);
                    }
                } else {
                    timeout = 0;
                    state = I2C_START;
                    printf("i2c bus is busy in READ!\n");
                    i2c_bus_reset();
                    return I2C_FAIL;
                }
            }
            i2c_start_on_bus(I2CX);
            timeout = 0;
            state = I2C_SEND_ADDRESS;
        }
        break;

        case I2C_SEND_ADDRESS: {
            while ((!i2c_flag_get(I2CX, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                if (RESET == read_cycle) {
                    i2c_master_addressing(I2CX, slave_address, I2C_TRANSMITTER);
                    state = I2C_CLEAR_ADDRESS_FLAG;
                } else {
                    i2c_master_addressing(I2CX, slave_address, I2C_RECEIVER);
                    if (number_of_byte < 3) {
                        i2c_ack_config(I2CX, I2C_ACK_DISABLE);
                    }
                    state = I2C_CLEAR_ADDRESS_FLAG;
                }
                timeout = 0;
            } else {
                timeout = 0;
                state = I2C_START;
                read_cycle = 0;
                i2c_bus_reset();
                timeout = 0;
                printf("i2c master sends start signal timeout in READ!\n");
            }
        }
        break;

        case I2C_CLEAR_ADDRESS_FLAG: {
            while ((!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND);
                if ((SET == read_cycle) && (1 == number_of_byte)) {
                    i2c_stop_on_bus(I2CX);
                }
                timeout = 0;
                state = I2C_TRANSMIT_DATA;
            } else {
                timeout = 0;
                state = I2C_START;
                read_cycle = 0;
                i2c_bus_reset();
                printf("i2c master clears address flag timeout in READ!\n");
                return I2C_FAIL;
            }
        }
        break;

        case I2C_TRANSMIT_DATA: {
            if (RESET == read_cycle) {
                while ((!i2c_flag_get(I2CX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if (timeout < I2C_TIME_OUT) {
                    i2c_data_transmit(I2CX, read_address);
                    timeout = 0;
                } else {
                    timeout = 0;
                    state = I2C_START;
                    read_cycle = 0;
                    printf("i2c master wait data buffer is empty timeout in READ!\n");
                }
                while ((!i2c_flag_get(I2CX, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if (timeout < I2C_TIME_OUT) {
                    timeout = 0;
                    state = I2C_START;
                    read_cycle++;
                } else {
                    timeout = 0;
                    state = I2C_START;
                    read_cycle = 0;
                    printf("i2c master sends EEPROM's internal address timeout in READ!\n");
                    i2c_bus_reset();
                    i2c_timeout_flag = I2C_OK;
                    return I2C_FAIL;
                }
            } else {
                while (number_of_byte) {
                    timeout++;
                    if (3 == number_of_byte) {
                        while (!i2c_flag_get(I2CX, I2C_FLAG_BTC))
                            ;
                        i2c_ack_config(I2CX, I2C_ACK_DISABLE);
                    }
                    if (2 == number_of_byte) {
                        while (!i2c_flag_get(I2CX, I2C_FLAG_BTC))
                            ;
                        i2c_stop_on_bus(I2CX);
                    }

                    if (i2c_flag_get(I2CX, I2C_FLAG_RBNE)) {
                        *p_buffer = i2c_data_receive(I2CX);
                        p_buffer++;
                        number_of_byte--;
                        timeout = 0;
                    }
                    if (timeout > I2C_TIME_OUT) {
                        timeout = 0;
                        state = I2C_START;
                        read_cycle = 0;
                        printf("i2c master sends data timeout in READ!\n");
                    }
                }
                timeout = 0;
                state = I2C_STOP;
            }
        }
        break;

        case I2C_STOP: {
            while ((I2C_CTL0(I2CX) & I2C_CTL0_STOP) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if (timeout < I2C_TIME_OUT) {
                timeout = 0;
                state = I2C_END;
                i2c_timeout_flag = I2C_OK;
            } else {
                timeout = 0;
                state = I2C_START;
                read_cycle = 0;
                printf("i2c master sends stop signal timeout in READ!\n");
            }
        }
        break;

        default: {
            state = I2C_START;
            read_cycle = 0;
            i2c_timeout_flag = I2C_OK;
            timeout = 0;
            printf("i2c master sends start signal in READ.\n");
        }
        break;
        }
    }
    return I2C_END;
}

uint8_t i2c_buffer_write(uint8_t *p_buffer, uint8_t write_address, uint16_t number_of_byte)
{
    uint8_t number_of_page = 0;
    uint8_t number_of_single = 0;
    uint8_t address = 0;
    uint8_t count = 0;

    address = write_address % I2C_PAGE_SIZE;
    count = I2C_PAGE_SIZE - address;
    number_of_page = number_of_byte / I2C_PAGE_SIZE;
    number_of_single = number_of_byte % I2C_PAGE_SIZE;
    uint8_t result = 3;

    if (0 == address) {
        while (number_of_page--) {
            result = i2c_page_write_timeout(p_buffer, write_address, I2C_PAGE_SIZE);
            if (I2C_OK != result)
                return result;
            result = i2c_wait_standby_state_timeout();
            if (I2C_OK != result)
                return result;
            write_address += I2C_PAGE_SIZE;
            p_buffer += I2C_PAGE_SIZE;
        }
        if (0 != number_of_single) {
            result = i2c_page_write_timeout(p_buffer, write_address, number_of_single);
            if (I2C_OK != result)
                return result;
            result = i2c_wait_standby_state_timeout();
            if (I2C_OK != result)
                return result;
        }
    } else {
        if (number_of_byte < count) {
            result = i2c_page_write_timeout(p_buffer, write_address, number_of_byte);
            if (I2C_OK != result)
                return result;
            result = i2c_wait_standby_state_timeout();
            if (I2C_OK != result)
                return result;
        } else {
            number_of_byte -= count;
            number_of_page = number_of_byte / I2C_PAGE_SIZE;
            number_of_single = number_of_byte % I2C_PAGE_SIZE;

            if (0 != count) {
                result = i2c_page_write_timeout(p_buffer, write_address, count);
                if (I2C_OK != result)
                    return result;
                result = i2c_wait_standby_state_timeout();
                if (I2C_OK != result)
                    return result;
                write_address += count;
                p_buffer += count;
            }
            while (number_of_page--) {
                result = i2c_page_write_timeout(p_buffer, write_address, I2C_PAGE_SIZE);
                if (I2C_OK != result)
                    return result;

                result = i2c_wait_standby_state_timeout();
                if (I2C_OK != result)
                    return result;
                write_address += I2C_PAGE_SIZE;
                p_buffer += I2C_PAGE_SIZE;
            }
            if (0 != number_of_single) {
                result = i2c_page_write_timeout(p_buffer, write_address, number_of_single);
                if (I2C_OK != result)
                    return result;

                result = i2c_wait_standby_state_timeout();
                if (I2C_OK != result)
                    return result;
            }
        }
    }
    return result;
}
