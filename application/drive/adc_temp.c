#include "adc_temp.h"

/**
  * @brief  初始化ADC和GPIO
  * @param  无
  * @retval 无
  */
void adc_temp_init(void)
{
    /* 使能GPIO时钟 */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    
    /* 使能ADC时钟 */
    rcu_periph_clock_enable(RCU_ADC0);
    
    /* 配置ADC时钟 */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);  // ADC时钟设为20MHz(120MHz/6)
    
    /* 配置GPIO为模拟输入模式 */
    gpio_init(TEMP1_GPIO_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, TEMP1_GPIO_PIN);
    gpio_init(TEMP2_GPIO_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, TEMP2_GPIO_PIN);
    gpio_init(DC_DC_TEMP_GPIO_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, DC_DC_TEMP_GPIO_PIN);
    gpio_init(TEMP3_GPIO_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, TEMP3_GPIO_PIN);
    gpio_init(TEMP4_GPIO_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, TEMP4_GPIO_PIN);
    gpio_init(TEMP5_GPIO_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, TEMP5_GPIO_PIN);
    gpio_init(VBAT_P_TEMP_GPIO_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, VBAT_P_TEMP_GPIO_PIN);
    gpio_init(VBAT_N_TEMP_GPIO_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, VBAT_N_TEMP_GPIO_PIN);
    
    /* ADC配置 */
    adc_deinit(ADC0);
    
    /* ADC模式配置 */
    adc_mode_config(ADC_MODE_FREE);
    
    /* 配置ADC分辨率 */
    adc_resolution_config(ADC0, ADC_RESOLUTION_12B);
    
    /* ADC扫描模式禁用 */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);
    
    /* ADC连续转换模式禁用 */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    
    /* 禁用外部触发 */
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, DISABLE);
    
    /* ADC触发配置为软件触发 */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    
    /* ADC数据对齐配置 */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    
    /* ADC通道长度配置 */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);
    
    /* 使能ADC */
    adc_enable(ADC0);
    
    /* 等待ADC稳定 */
    delay_1ms(1);
    
    /* ADC校准 */
    adc_calibration_enable(ADC0);
}

/**
  * @brief  获取指定通道的ADC值
  * @param  channel: ADC通道
  * @retval ADC转换结果，返回多次采样的平均值
  */
static uint16_t get_adc_value(uint8_t channel)
{
    uint32_t total = 0;
    const uint8_t samples = 16;  // 采样次数
    
    for(uint8_t i = 0; i < samples; i++) {
        /* 配置规则通道转换顺序 */
        adc_regular_channel_config(ADC0, 0, channel, ADC_SAMPLETIME_239POINT5);
        
        /* 软件触发ADC转换 */
        adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
        
        /* 等待转换完成 */
        while(!adc_flag_get(ADC0, ADC_FLAG_EOC));
        
        /* 读取ADC值并累加 */
        total += adc_regular_data_read(ADC0);
        
        /* 清除EOC标志 */
        adc_flag_clear(ADC0, ADC_FLAG_EOC);
        
        /* 短暂延时 */
        delay_1ms(1);
    }
    
    /* 返回平均值 */
    return (uint16_t)(total / samples);
}

/* 获取各个温度传感器的ADC值 */
uint16_t get_temp1_adc(void)
{
    return get_adc_value(TEMP1_CHANNEL);
}

uint16_t get_temp2_adc(void)
{
    return get_adc_value(TEMP2_CHANNEL);
}

uint16_t get_dc_dc_temp_adc(void)
{
    return get_adc_value(DC_DC_TEMP_CHANNEL);
}

uint16_t get_temp3_adc(void)
{
    return get_adc_value(TEMP3_CHANNEL);
}

uint16_t get_temp4_adc(void)
{
    return get_adc_value(TEMP4_CHANNEL);
}

uint16_t get_temp5_adc(void)
{
    return get_adc_value(TEMP5_CHANNEL);
}

uint16_t get_vbat_p_temp_adc(void)
{
    return get_adc_value(VBAT_P_TEMP_CHANNEL);
}

uint16_t get_vbat_n_temp_adc(void)
{
    return get_adc_value(VBAT_N_TEMP_CHANNEL);
}
