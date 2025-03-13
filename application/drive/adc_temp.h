#ifndef ADC_TEMP_H
#define ADC_TEMP_H

#include "gd32f30x.h"

/* 温度传感器通道定义 */
#define TEMP1_CHANNEL        ADC_CHANNEL_8      // PB0  ADC01_IN8
#define TEMP2_CHANNEL        ADC_CHANNEL_9      // PB1  ADC01_IN9
#define DC_DC_TEMP_CHANNEL   ADC_CHANNEL_3      // PA3  ADC012_IN3
#define TEMP3_CHANNEL        ADC_CHANNEL_10     // PC0  ADC012_IN10
#define TEMP4_CHANNEL        ADC_CHANNEL_12     // PC2  ADC012_IN12
#define TEMP5_CHANNEL        ADC_CHANNEL_13     // PC3  ADC012_IN13
#define VBAT_P_TEMP_CHANNEL  ADC_CHANNEL_14     // PC4  ADC01_IN14
#define VBAT_N_TEMP_CHANNEL  ADC_CHANNEL_15     // PC5  ADC01_IN15

/* 温度传感器GPIO定义 */
#define TEMP1_GPIO_PORT      GPIOB
#define TEMP1_GPIO_PIN       GPIO_PIN_0
#define TEMP2_GPIO_PORT      GPIOB
#define TEMP2_GPIO_PIN       GPIO_PIN_1
#define DC_DC_TEMP_GPIO_PORT GPIOA
#define DC_DC_TEMP_GPIO_PIN  GPIO_PIN_3
#define TEMP3_GPIO_PORT      GPIOC
#define TEMP3_GPIO_PIN       GPIO_PIN_0
#define TEMP4_GPIO_PORT      GPIOC
#define TEMP4_GPIO_PIN       GPIO_PIN_2
#define TEMP5_GPIO_PORT      GPIOC
#define TEMP5_GPIO_PIN       GPIO_PIN_3
#define VBAT_P_TEMP_GPIO_PORT GPIOC
#define VBAT_P_TEMP_GPIO_PIN  GPIO_PIN_4
#define VBAT_N_TEMP_GPIO_PORT GPIOC
#define VBAT_N_TEMP_GPIO_PIN  GPIO_PIN_5

/* 函数声明 */
void adc_temp_init(void);
uint16_t get_temp1_adc(void);
uint16_t get_temp2_adc(void);
uint16_t get_dc_dc_temp_adc(void);
uint16_t get_temp3_adc(void);
uint16_t get_temp4_adc(void);
uint16_t get_temp5_adc(void);
uint16_t get_vbat_p_temp_adc(void);
uint16_t get_vbat_n_temp_adc(void);

#endif /* ADC_TEMP_H */
