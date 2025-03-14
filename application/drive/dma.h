#ifndef DMA_H
#define DMA_H

#include "gd32f30x.h"

/* DMA通道定义 */
#define DMA_ADC_CHANNEL     DMA_CH0    // ADC使用DMA通道0
#define DMA_SPI_TX_CHANNEL  DMA_CH2    // SPI TX使用DMA通道2
#define DMA_SPI_RX_CHANNEL  DMA_CH1    // SPI RX使用DMA通道1

/* 函数声明 */
void dma_config(void);
FlagStatus dma_transfer_complete_flag_get(uint32_t dma_channel);
void dma_transfer_complete_flag_clear(uint32_t dma_channel);

#endif /* DMA_H */
