#include "adc_temp.h"

/* ADC DMA缓冲区 */
static uint16_t adc_values[ADC_CHANNEL_COUNT];

static volatile uint8_t adc_data_ready = 0;

/* ADC通道配置数组 */
static const uint8_t adc_channels[ADC_CHANNEL_COUNT] = {
    TEMP1_CHANNEL,
    TEMP2_CHANNEL,
    DC_DC_TEMP_CHANNEL,
    TEMP3_CHANNEL,
    TEMP4_CHANNEL,
    TEMP5_CHANNEL,
    VBAT_P_TEMP_CHANNEL,
    VBAT_N_TEMP_CHANNEL
};

/**
  * @brief  配置ADC的DMA传输
  * @param  无
  * @retval 无
  */
void adc_dma_config(void)
{
    /* 使能DMA时钟 */
    rcu_periph_clock_enable(RCU_DMA0);
    
    /* 配置DMA参数结构体 */
    dma_parameter_struct dma_init_struct;
    dma_struct_para_init(&dma_init_struct);
    
    /* 初始化ADC的DMA通道 */
    dma_deinit(DMA0, DMA_ADC_CHANNEL);
    
    /* 配置外设地址和数据宽度 */
    dma_init_struct.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    
    /* 配置内存地址和数据宽度 */
    dma_init_struct.memory_addr = (uint32_t)adc_values;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    
    /* 配置传输参数 */
    dma_init_struct.number = ADC_CHANNEL_COUNT;
    dma_init_struct.priority = DMA_PRIORITY_HIGH;
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    
    /* 初始化ADC DMA */
    dma_init(DMA0, DMA_ADC_CHANNEL, &dma_init_struct);
    
    /* 使能ADC DMA循环模式 */
    dma_circulation_enable(DMA0, DMA_ADC_CHANNEL);
    
    /* 使能ADC DMA传输完成中断 */
    dma_interrupt_enable(DMA0, DMA_ADC_CHANNEL, DMA_INT_FTF);
    
    /* 使能ADC DMA通道 */
    dma_channel_enable(DMA0, DMA_ADC_CHANNEL);
}

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
    
    /* 使能ADC扫描模式 */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    
    /* 使能ADC连续转换模式 */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
    
    /* 配置ADC DMA */
    adc_dma_config();
    
    /* ADC触发配置为软件触发 */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
    
    /* ADC数据对齐配置 */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    
    /* 配置ADC常规通道组 */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, ADC_CHANNEL_COUNT);
    
    /* 配置ADC采样时间和转换顺序 */
    for(uint8_t i = 0; i < ADC_CHANNEL_COUNT; i++) {
        adc_regular_channel_config(ADC0, i, adc_channels[i], ADC_SAMPLETIME_239POINT5);
    }
    
    /* 使能ADC */
    adc_enable(ADC0);
    
    /* 等待ADC稳定 */
    delay_1ms(1);
    
    /* ADC校准 */
    adc_calibration_enable(ADC0);
    
    /* 启动ADC转换 */
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}

/**
  * @brief  获取ADC值数组指针
  * @param  无
  * @retval ADC值数组指针
  */
uint16_t* get_adc_values(void)
{
    return adc_values;
}

/**
  * @brief  获取指定通道的电压值
  * @param  channel: 通道索引(0-7)
  * @retval 电压值(V)
  */
float get_channel_voltage(uint8_t channel)
{
    if(channel >= ADC_CHANNEL_COUNT) {
        return 0.0f;
    }
    return (float)adc_values[channel] * ADC_VREF / 4096.0f;
}

/**
  * @brief  检查ADC数据是否准备好
  * @param  无
  * @retval 1:准备好 0:未准备好
  */
uint8_t is_adc_data_ready(void)
{
    if(dma_flag_get(DMA0, DMA_ADC_CHANNEL, DMA_FLAG_FTF)) {
        dma_flag_clear(DMA0, DMA_ADC_CHANNEL, DMA_FLAG_FTF);
        adc_data_ready = 1;
    }
    return adc_data_ready;
}

/**
  * @brief  清除ADC数据就绪标志
  * @param  无
  * @retval 无
  */
void clear_adc_data_ready(void)
{
    adc_data_ready = 0;
}
