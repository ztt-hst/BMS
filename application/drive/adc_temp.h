#ifndef ADC_TEMP_H
#define ADC_TEMP_H

#include "gd32f30x.h"

/* ADC通道数量定义 */
#define ADC_CHANNEL_COUNT    8    // 总共8个温度传感器通道

/* ADC参考电压 */
#define ADC_VREF            3.3f  // ADC参考电压为3.3V

/* ADC DMA通道定义 */
#define DMA_ADC_CHANNEL     DMA_CH0  // ADC使用DMA通道0

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

/* 温度传感器通道索引枚举 */
typedef enum {
    TEMP1_INDEX = 0,
    TEMP2_INDEX,
    DC_DC_TEMP_INDEX,
    TEMP3_INDEX,
    TEMP4_INDEX,
    TEMP5_INDEX,
    VBAT_P_TEMP_INDEX,
    VBAT_N_TEMP_INDEX,
    TEMP_INDEX_MAX
} temp_channel_index_enum;

/* 函数声明 */
void adc_temp_init(void);
void adc_dma_config(void);
uint16_t* get_adc_values(void);
float get_channel_voltage(uint8_t channel);
uint8_t is_adc_data_ready(void);
void clear_adc_data_ready(void);

#endif /* ADC_TEMP_H */
