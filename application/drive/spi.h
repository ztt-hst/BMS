#ifndef SPI_H
#define SPI_H

#define SPI_CRC_ENABLE             1
#define ARRAYSIZE                  10
#define SET_SPI0_NSS_HIGH          gpio_bit_set(GPIOB,GPIO_PIN_15);
#define SET_SPI0_NSS_LOW           gpio_bit_reset(GPIOB,GPIO_PIN_15);

uint8_t spi0_send_array[ARRAYSIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};
uint8_t spi2_send_array[ARRAYSIZE] = {0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA};
uint8_t spi0_receive_array[ARRAYSIZE]; 
uint8_t spi2_receive_array[ARRAYSIZE];

void spi0_rcu_config(void);
void spi0_gpio_config(void);
void spi0_struct_config(void);
void spi0_dma_config(void);
void spi0_init(void);
void spi0_dma_test(void);


#endif //SPI_H