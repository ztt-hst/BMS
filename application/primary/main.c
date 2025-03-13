#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gd32f30x.h"
#include "gd32f303e_eval.h"
#include "systick.h"
#include "pu_table.h"
#include "i2c.h"
#include "bq76952.h"
#include "process.h"
#include "wdi.h"  


void usart_config(void) {
    rcu_periph_clock_enable(RCU_USART0);
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);  // TX
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);  // RX

    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
}

int fputc(int ch, FILE *f) {
    usart_data_transmit(USART0, (uint8_t)ch);
    while (usart_flag_get(USART0, USART_FLAG_TBE) == RESET);
    return ch;
}

int main(void) {
    uint8_t buffer[256];
    uint16_t temp_values[8];  // 存储8个温度传感器的ADC值
    
    usart_config();  // 初始化串口
    printf("USART initialized!\n");
    
    /* 初始化WDI */
    WDI_Init();
    printf("WDI initialized!\n");

    /* 初始化自动喂狗定时器 */
    WDI_AutoFeed_Init();
    printf("WDI Timer initialized!\n");
    
    /* 启动自动喂狗 */
    WDI_AutoFeed_Start();
    printf("WDI Start!\n");

    /* 初始化ADC温度采集 */
    adc_temp_init();
    printf("ADC Temperature initialized!\n");

    while (1) {
        /* 采集所有温度传感器的值 */
        temp_values[0] = get_temp1_adc();
        temp_values[1] = get_temp2_adc();
        temp_values[2] = get_dc_dc_temp_adc();
        temp_values[3] = get_temp3_adc();
        temp_values[4] = get_temp4_adc();
        temp_values[5] = get_temp5_adc();
        temp_values[6] = get_vbat_p_temp_adc();
        temp_values[7] = get_vbat_n_temp_adc();

        /* 打印所有温度值 */
        printf("\r\n============ Temperature Values ============\r\n");
        printf("TEMP1 (PB0): %4d\r\n", temp_values[0]);
        printf("TEMP2 (PB1): %4d\r\n", temp_values[1]);
        printf("DC_DC_TEMP (PA3): %4d\r\n", temp_values[2]);
        printf("TEMP3 (PC0): %4d\r\n", temp_values[3]);
        printf("TEMP4 (PC2): %4d\r\n", temp_values[4]);
        printf("TEMP5 (PC3): %4d\r\n", temp_values[5]);
        printf("VBAT+_TEMP (PC4): %4d\r\n", temp_values[6]);
        printf("VBAT-_TEMP (PC5): %4d\r\n", temp_values[7]);
        printf("=========================================\r\n");

        /* 延时1秒 */
        delay_1ms(1000);
    }
}

