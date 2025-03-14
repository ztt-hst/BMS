#include "dma.h"
#include "adc_temp.h"

extern uint16_t adc_values[ADC_CHANNEL_COUNT];  // 从adc_temp.c中声明

/**
  * @brief  统一初始化所有需要的DMA通道
  * @param  无
  * @retval 无
  */
void dma_config(void)
{
    /* 使能DMA时钟 */
    rcu_periph_clock_enable(RCU_DMA0);
    
    /* 配置DMA参数结构体 */
    dma_parameter_struct dma_init_struct;
    dma_struct_para_init(&dma_init_struct);
    
    /* ====== ADC DMA配置 ====== */
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
    
    /* 使能ADC DMA通道 */
    dma_channel_enable(DMA0, DMA_ADC_CHANNEL);

}

/**
  * @brief  获取指定通道的DMA传输完成标志
  * @param  dma_channel: DMA通道
  * @retval SET或RESET
  */
FlagStatus dma_transfer_complete_flag_get(uint32_t dma_channel)
{
    return dma_flag_get(DMA0, dma_channel, DMA_FLAG_FTF);
}

/**
  * @brief  清除指定通道的DMA传输完成标志
  * @param  dma_channel: DMA通道
  * @retval 无
  */
void dma_transfer_complete_flag_clear(uint32_t dma_channel)
{
    dma_flag_clear(DMA0, dma_channel, DMA_FLAG_FTF);
}

// 其他外设的DMA配置函数可以在这里添加
