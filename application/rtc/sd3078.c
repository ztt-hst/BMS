#include "sd3078.h"

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

#define GPIOC_BASE              (APB2_BUS_BASE+0x1000)
#define GPIOC_ODR_Addr          (GPIOC_BASE+12)

#define PCout(n)                BIT_ADDR(GPIOC_ODR_Addr,n)

#define S_SET_IIC_SDA_OUT()     {GPIO_CTL1(GPIOC)&=0xFFFF0FFF;GPIO_CTL1(GPIOC)|=(uint32_t)3<<12;}
#define S_SET_IIC_SDA_IN()      {GPIO_CTL1(GPIOC)&=0xFFFF0FFF;GPIO_CTL1(GPIOC)|=(uint32_t)4<<12;}

#define S_IIC_SCL               PCout(12)
#define S_IIC_SDA_OUT           PCout(11)

#define sd3078_device_addr      (0x64)
#define SD3078_CTR1           	(0x0F)
#define SD3078_CTR2             (0x10)  //enable
#define SD3078_CTR3           	(0x11)
#define SD3078_CTR4				(0x19)
#define SD3078_TEMPL    		(0x16)
#define sd3078_time_add			(0x00)
#define sd3078_user_ram			(0x71)


#define B2H						(0)
#define H2B					    (1)

static void IIC_Start(void)
{
    S_SET_IIC_SDA_OUT();
    S_IIC_SDA_OUT=1;
    S_IIC_SCL=1;
    delay_1ms(1);
    S_IIC_SDA_OUT=0;
    delay_1ms(1);
    S_IIC_SCL=0;
}

static void IIC_Stop(void)
{
    S_SET_IIC_SDA_OUT();
    S_IIC_SCL=0;
    S_IIC_SDA_OUT=0;
    delay_1ms(1);
    S_IIC_SCL=1;
    delay_1ms(1);
    S_IIC_SDA_OUT=1;
    delay_1ms(1);
}

static uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime=0;
    S_SET_IIC_SDA_IN();
    S_IIC_SDA_OUT=1;
    delay_1ms(1);
    S_IIC_SCL=1;
    delay_1ms(1);
    while(S_IIC_SDA_OUT) {
        ucErrTime++;
        if(ucErrTime>250) {
            IIC_Stop();
            return 1;
        }
    }
    S_IIC_SCL=0;
    return 0;
}

static void IIC_Ack(void)
{
    S_IIC_SCL=0;
    S_SET_IIC_SDA_OUT();
    S_IIC_SDA_OUT=0;
    delay_1ms(1);
    S_IIC_SCL=1;
    delay_1ms(1);
    S_IIC_SCL=0;
}

static void IIC_NAck(void)
{
    S_IIC_SCL=0;
    S_SET_IIC_SDA_OUT();
    S_IIC_SDA_OUT=1;
    delay_1ms(1);
    S_IIC_SCL=1;
    delay_1ms(1);
    S_IIC_SCL=0;
}

static void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    S_SET_IIC_SDA_OUT();
    S_IIC_SCL=0;
    for(t=0; t<8; t++) {
        S_IIC_SDA_OUT=(txd&0x80)>>7;
        txd<<=1;
        delay_1ms(1);
        S_IIC_SCL=1;
        delay_1ms(1);
        S_IIC_SCL=0;
        delay_1ms(1);
    }
}

static uint8_t IIC_Read_Byte(uint8_t ack)
{
    uint8_t i,receive=0;
    S_SET_IIC_SDA_IN();
    for(i=0; i<8; i++ ) {
        S_IIC_SCL=0;
        delay_1ms(1);
        S_IIC_SCL=1;
        receive<<=1;
        if(S_IIC_SDA_OUT)
            receive++;
        delay_1ms(1);
    }
    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();
    return receive;
}

static uint8_t TOHEX(uint8_t mod, uint8_t value)
{
    uint8_t i, j, k;
    if(mod == B2H) {
        i = value & 0x0F;
        value >>= 4;
        value &= 0x0F;
        value *= 10;
        i += value;
        return i;
    } else {
        i = value / 10;
        j = value % 10;
        k = j + (i << 4);
        return k;
    }
}


void sd3078_gpio_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);

    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    S_SET_IIC_SDA_OUT();
    S_IIC_SCL=1;
    S_IIC_SDA_OUT=1;
}

void sd3078_write_enable(void)
{
    IIC_Start();
    IIC_Send_Byte(sd3078_device_addr);
    IIC_Wait_Ack();
    IIC_Send_Byte(SD3078_CTR2);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x80);
    IIC_Wait_Ack();
    IIC_Stop();

    IIC_Start();
    IIC_Send_Byte(sd3078_device_addr);
    IIC_Wait_Ack();
    IIC_Send_Byte(SD3078_CTR1);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x84);
    IIC_Wait_Ack();
    IIC_Stop();
}

void sd3078_write_disable(void)
{
    IIC_Start();
    IIC_Send_Byte(sd3078_device_addr);
    IIC_Wait_Ack();
    IIC_Send_Byte(SD3078_CTR1);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x0);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x0);
    IIC_Wait_Ack();
    IIC_Stop();
}

void sd3078_write_data(uint8_t addr, uint8_t length, uint8_t *data)
{
    sd3078_write_enable();
    IIC_Start();
    IIC_Send_Byte(sd3078_device_addr);
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);
    IIC_Wait_Ack();
    uint8_t i = 0;

    for (i = 0; i < length; i++) {
        IIC_Send_Byte(data[i]);
        IIC_Wait_Ack();
    }

    IIC_Stop();
    sd3078_write_disable();
}

void sd3078_read_data(uint8_t addr, uint8_t length, uint8_t *data)
{
    uint8_t i;
    uint8_t ack_flag;
    IIC_Start();
    IIC_Send_Byte(sd3078_device_addr);
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(sd3078_device_addr+1);
    IIC_Wait_Ack();

    for (i = 0; i < length; i++) {
        ack_flag = (i == length - 1) ? 0 : 1;
        data[i] = IIC_Read_Byte(ack_flag);
    }

    IIC_Stop();
}

void sd3078_set_time(uint8_t *time)
{
    uint8_t i = 0;
    sd3078_read_data(sd3078_time_add, 7, time);
    for (i = 0; i < 7; i++) {
        if(i == 2)
            time[i] &= 0x7F;
        time[i] = TOHEX(B2H, time[i]);
    }
}

void sd3078_get_time(uint8_t *time)
{
    uint8_t i = 0;
    for(i = 0; i < 7; i++) {
        time[i] = TOHEX(H2B, time[i]);
        if(i == 2)
            time[i] |= 0x80;
    }
    sd3078_write_data(sd3078_time_add, 7, time);
}

uint8_t sd3078_get_status(void)
{
    uint8_t temp = 0;
    sd3078_read_data(sd3078_user_ram, 1, &temp);
    if(temp == 0x55)
        return 0;
    else {
        temp = 0x55;
        sd3078_write_data(sd3078_user_ram, 1, &temp);
        temp = 0;
        sd3078_read_data(sd3078_user_ram, 1, &temp);
        if(temp == 0x55)
            return 0;
    }

    return 1;
}


