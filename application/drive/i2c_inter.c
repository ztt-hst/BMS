#include <stddef.h>
#include <stdbool.h>
#include "gd32f30x.h"
#include "gd32f30x_libopt.h"

#define I2C_DEV      I2C0
#define I2CClockSpeed (400000)
#define I2CLocalAddress (0x22)

typedef enum {
    I2C_OP_NONE = 0,
    I2C_OP_WRITE,
    I2C_OP_READ
} i2c_operation_t;

typedef enum {
    I2C_STATE_IDLE = 0,

    I2C_STATE_WRITE_START,
    I2C_STATE_WRITE_SEND_ADDR_W,
    I2C_STATE_WRITE_ADDR_W_ACK,
    I2C_STATE_WRITE_SEND_REG,
    I2C_STATE_WRITE_REG_WAIT,
    I2C_STATE_WRITE_SEND_DATA,
    I2C_STATE_WRITE_DATA_WAIT,
    I2C_STATE_WRITE_STOP,

    I2C_STATE_READ_START_1,
    I2C_STATE_READ_SEND_ADDR_W,
    I2C_STATE_READ_ADDR_W_ACK,
    I2C_STATE_READ_SEND_REG,
    I2C_STATE_READ_REG_WAIT,
    I2C_STATE_READ_START_2,
    I2C_STATE_READ_SEND_ADDR_R,
    I2C_STATE_READ_ADDR_R_ACK,
    I2C_STATE_READ_DATA,
    I2C_STATE_READ_3_LEFT_WAIT_BTC,
    I2C_STATE_READ_STOP
} i2c_sub_state_t;

static volatile i2c_operation_t g_i2cOp      = I2C_OP_NONE;
static volatile i2c_sub_state_t g_i2cState   = I2C_STATE_IDLE;
static volatile bool            g_i2cBusy    = false;
static volatile bool            g_i2cSuccess = false;

static volatile uint8_t  g_slaveAddr = 0x10;
static volatile uint8_t  g_regAddr   = 0;

static volatile uint8_t *g_i2cTxBuf  = NULL;
static volatile uint32_t g_txCount   = 0;

static volatile uint8_t *g_i2cRxBuf  = NULL;
static volatile uint32_t g_rxCount   = 0;

void i2c_interrupt_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_I2C0);

    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    gpio_pin_remap_config(GPIO_I2C0_REMAP, DISABLE);

    i2c_clock_config(I2C_DEV, I2CClockSpeed, I2C_DTCY_2);
    i2c_mode_addr_config(I2C_DEV, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2CLocalAddress);
    i2c_enable(I2C_DEV);
    i2c_ack_config(I2C_DEV, I2C_ACK_ENABLE);

    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(I2C0_EV_IRQn, 0, 3);
    nvic_irq_enable(I2C0_ER_IRQn, 0, 2);
}

void i2c_interrupt_deinit(void)
{
    i2c_disable(I2C_DEV);
    i2c_ack_config(I2C_DEV, I2C_ACK_DISABLE);

    nvic_irq_disable(I2C0_EV_IRQn);
    nvic_irq_disable(I2C0_ER_IRQn);

    rcu_periph_clock_disable(RCU_I2C0);
    rcu_periph_clock_disable(RCU_AF);
    rcu_periph_clock_disable(RCU_GPIOB);
    gpio_deinit(GPIOB);
}

void i2c_master_write_reg(uint8_t devAddr, uint8_t regAddr, uint8_t *pData, uint32_t length)
{
    if (g_i2cBusy) {
        return;
    }

    g_i2cBusy     = true;
    g_i2cSuccess  = false;

    g_i2cOp       = I2C_OP_WRITE;
    g_i2cState    = I2C_STATE_WRITE_START;

    g_slaveAddr   = devAddr;
    g_regAddr     = regAddr;
    g_i2cTxBuf    = pData;
    g_txCount     = length;

    i2c_interrupt_enable(I2C_DEV, I2C_INT_EV);
    i2c_interrupt_enable(I2C_DEV, I2C_INT_ERR);
    i2c_interrupt_enable(I2C_DEV, I2C_INT_BUF);

    i2c_start_on_bus(I2C_DEV);
}

void i2c_master_read_reg(uint8_t devAddr, uint8_t regAddr, uint8_t *pBuf, uint32_t length)
{
    if (g_i2cBusy) {
        return;
    }

    g_i2cBusy     = true;
    g_i2cSuccess  = false;

    g_i2cOp       = I2C_OP_READ;
    g_i2cState    = I2C_STATE_READ_START_1;

    g_slaveAddr   = devAddr;
    g_regAddr     = regAddr;

    g_i2cRxBuf    = pBuf;
    g_rxCount     = length;

    i2c_interrupt_enable(I2C_DEV, I2C_INT_EV);
    i2c_interrupt_enable(I2C_DEV, I2C_INT_ERR);
    i2c_interrupt_enable(I2C_DEV, I2C_INT_BUF);

    i2c_start_on_bus(I2C_DEV);
}

void I2C0_EV_IRQHandler(void)
{
    switch(g_i2cState) {
    case I2C_STATE_READ_START_1:
        if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_SBSEND)) {
            i2c_master_addressing(I2C_DEV, g_slaveAddr, I2C_TRANSMITTER);
            g_i2cState = I2C_STATE_READ_SEND_ADDR_W;
            printf("I2C_STATE_READ_START_1\n");
        }
        break;

    case I2C_STATE_READ_SEND_ADDR_W:
        if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_ADDSEND)) {
            i2c_interrupt_flag_clear(I2C_DEV, I2C_INT_FLAG_ADDSEND);
            g_i2cState = I2C_STATE_READ_ADDR_W_ACK;
            printf("I2C_STATE_READ_SEND_ADDR_W\n");
        }
        break;

    case I2C_STATE_READ_ADDR_W_ACK:
        if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_TBE)) {
            i2c_data_transmit(I2C_DEV, g_regAddr);
            g_i2cState = I2C_STATE_READ_SEND_REG;
            printf("I2C_STATE_READ_ADDR_W_ACK\n");
        }
        break;

    case I2C_STATE_READ_SEND_REG:
        if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_BTC)) {
            i2c_start_on_bus(I2C_DEV);
            g_i2cState = I2C_STATE_READ_START_2;
            printf("I2C_STATE_READ_SEND_REG\n");
        }
        break;

    case I2C_STATE_READ_START_2:
        if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_SBSEND)) {
            i2c_master_addressing(I2C_DEV, g_slaveAddr, I2C_RECEIVER);
            g_i2cState = I2C_STATE_READ_SEND_ADDR_R;
            printf("I2C_STATE_READ_START_2\n");
        }
        break;

    case I2C_STATE_READ_SEND_ADDR_R:
        if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_ADDSEND)) {
            i2c_interrupt_flag_clear(I2C_DEV, I2C_INT_FLAG_ADDSEND);
            if(g_rxCount <= 2) {
                i2c_ack_config(I2C_DEV, I2C_ACK_DISABLE);
            }
            g_i2cState = I2C_STATE_READ_DATA;
            printf("I2C_STATE_READ_SEND_ADDR_R\n");
        }
        break;

    case I2C_STATE_READ_DATA:
        if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_RBNE)) {
            *g_i2cRxBuf++ = i2c_data_receive(I2C_DEV);
            g_rxCount--;
            printf("Read data: %d\n", *g_i2cRxBuf);
            if(g_rxCount == 3) {
                g_i2cState = I2C_STATE_READ_3_LEFT_WAIT_BTC;
            } else if(g_rxCount == 0) {
                i2c_stop_on_bus(I2C_DEV);
                g_i2cState = I2C_STATE_READ_STOP;
            }
            printf("I2C_STATE_READ_DATA\n");
        }
        break;

    case I2C_STATE_READ_3_LEFT_WAIT_BTC:
        if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_BTC)) {
            i2c_ack_config(I2C_DEV, I2C_ACK_DISABLE);
            g_i2cState = I2C_STATE_READ_DATA;
            printf("I2C_STATE_READ_3_LEFT_WAIT_BTC\n");
        }
        break;

    case I2C_STATE_READ_STOP:
        printf("I2C_STATE_READ_STOP\n");
        g_i2cBusy    = false;
        g_i2cSuccess = true;

        i2c_ack_config(I2C_DEV, I2C_ACK_ENABLE);
        i2c_ackpos_config(I2C_DEV, I2C_ACKPOS_CURRENT);

        g_i2cState = I2C_STATE_IDLE;
        g_i2cOp    = I2C_OP_NONE;

        i2c_interrupt_disable(I2C_DEV, I2C_INT_EV);
        i2c_interrupt_disable(I2C_DEV, I2C_INT_ERR);
        i2c_interrupt_disable(I2C_DEV, I2C_INT_BUF);
        break;

    default:
        break;
    }
}

void I2C0_ER_IRQHandler(void)
{
    if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_AERR)) {
        printf("send and receive data error %d.\n", I2C_INT_FLAG_AERR);
        i2c_interrupt_flag_clear(I2C_DEV, I2C_INT_FLAG_AERR);
    }

    if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_SMBALT)) {
        printf("send and receive data error %d.\n", I2C_INT_FLAG_SMBALT);
        i2c_interrupt_flag_clear(I2C_DEV, I2C_INT_FLAG_SMBALT);
    }

    if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_SMBTO)) {
        printf("send and receive data error %d.\n", I2C_INT_FLAG_SMBTO);
        i2c_interrupt_flag_clear(I2C_DEV, I2C_INT_FLAG_SMBTO);
    }

    if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_OUERR)) {
        printf("send and receive data error %d.\n", I2C_INT_FLAG_OUERR);
        i2c_interrupt_flag_clear(I2C_DEV, I2C_INT_FLAG_OUERR);
    }

    if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_LOSTARB)) {
        printf("send and receive data error %d.\n", I2C_INT_FLAG_LOSTARB);
        i2c_interrupt_flag_clear(I2C_DEV, I2C_INT_FLAG_LOSTARB);
    }

    if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_BERR)) {
        printf("send and receive data error %d.\n", I2C_INT_FLAG_BERR);
        i2c_interrupt_flag_clear(I2C_DEV, I2C_INT_FLAG_BERR);
    }

    if(i2c_interrupt_flag_get(I2C_DEV, I2C_INT_FLAG_PECERR)) {
        printf("send and receive data error %d.\n", I2C_INT_FLAG_PECERR);
        i2c_interrupt_flag_clear(I2C_DEV, I2C_INT_FLAG_PECERR);

    }

    i2c_interrupt_disable(I2C_DEV, I2C_INT_EV);
    i2c_interrupt_disable(I2C_DEV, I2C_INT_ERR);
    i2c_interrupt_disable(I2C_DEV, I2C_INT_BUF);
    i2c_interrupt_deinit();
    i2c_interrupt_init();
}
