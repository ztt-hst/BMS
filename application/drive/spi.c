#include <stdio.h>
#include "spi.h"
#include "gd32f30x.h"

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi0_rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_AF);
}


/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi0_gpio_config(void)
{
    /* SPI0 GPIO config: SCK/PA5, MISO/PA6, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    /* PB15 as NSS */
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi0_struct_config(void)
{
    spi_parameter_struct spi_init_struct;

    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_256;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi0_dma_config(void)
{
    dma_parameter_struct dma_init_struct;
    
    /* SPI0 transmit dma config:DMA0-DMA_CH2  */
    dma_deinit(DMA0, DMA_CH2);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI0);
    dma_init_struct.memory_addr  = (uint32_t)spi0_send_array;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_LOW;
    dma_init_struct.number       = ARRAYSIZE;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA0, DMA_CH2, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH2);
    dma_memory_to_memory_disable(DMA0, DMA_CH2);

    /* SPI0 receive dma config:DMA0-DMA_CH1  */
    dma_deinit(DMA0, DMA_CH1);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI0);
    dma_init_struct.memory_addr  = (uint32_t)spi0_receive_array;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH1, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH1);
    dma_memory_to_memory_disable(DMA0, DMA_CH1);
}

void spi0_init(void)
{
    spi0_rcu_config();
    spi0_gpio_config();
    spi0_dma_config();
    spi0_struct_config();
    SET_SPI0_NSS_HIGH
    spi_enable(SPI0);
}

void spi0_dma_test(void)
{
    spi0_init();

    dma_channel_enable(DMA0, DMA_CH1);
    dma_channel_enable(DMA0, DMA_CH2);
    SET_SPI0_NSS_LOW
    spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
    spi_dma_enable(SPI0, SPI_DMA_RECEIVE);

    /* wait dma transmit complete */
    while(!dma_flag_get(DMA0,DMA_CH2, DMA_INTF_FTFIF)) {
    }
    while(!dma_flag_get(DMA0,DMA_CH1, DMA_INTF_FTFIF)) {
    }

    SET_SPI0_NSS_HIGH

    if(SET != spi_i2s_flag_get(SPI0, SPI_FLAG_CRCERR)) {
        printf("SPI0 Receive Array:\r\n");
        for (int i = 0; i < ARRAYSIZE; i++) {
            printf("%02X ", spi0_receive_array[i]);
        }
        printf("\r\n");
    } else {
        printf("SPI0 error\n");
    }
    while(1);
}