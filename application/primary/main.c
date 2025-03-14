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
#include "sd3078.h"
#include "wdi.h"  
#include "adc_temp.h"
#include "dma.h"


#define BASE_ADDRESS_1 0x1000
#define BASE_OFFSET_1  0x0110
#define BASE_ADDRESS_2 0x1200
#define BASE_OFFSET_2  0x0100
#define BASE_ADDRESS_3 0x1400
#define BASE_OFFSET_3  0x0024
#define BASE_ADDRESS_4 0x1600
#define BASE_OFFSET_4  0x000E

float kelvin_to_celsius(uint16_t kelvin)
{
    float celsius = kelvin / 10.0;
    return (celsius - 273.15);
}

void init_build_time(void)
{
    PRINT_ERROR("Build time is: %s\n", BUILD_TIME);
}

void process_base_address(uint32_t base_address, uint32_t base_offset, int step)
{
    uint32_t result;
    int i = 0;
    for (i = 0; i < base_offset; i += step) {
        result = pu_search_id(base_address, i);
        if (result != 0) {
            //direct command read register
            if(pu_get_status(result) == 1) {
                pu_set_status(result, 0);
                send_replay_data(result);
            }
            //direct command write register
            if(pu_get_status(result) == 10) {
                pu_set_status(result, 0);
                write_command_data(result);
            }
        }
    }
}
static uint32_t application_address = 0x08008000;
uint32_t verify_firmware(void)
{
    /*initial CRC clock*/
    rcu_periph_clock_enable(RCU_CRC);
    crc_data_register_reset();
    uint32_t current_block_number = 25;
    uint32_t page_size = 2048;
    uint32_t *papp_address = (__IO uint32_t *)application_address;
    uint32_t crc_result = crc_block_data_calculate(papp_address, current_block_number * page_size);
    //printf("crc_result: 0x%08X\n", crc_result);
    return crc_result;
}

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

int main(int argc, char **argv)
{
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, MAPP_START_ADDRESS);
    __enable_irq();

    SystemInit();
    SystemCoreClockUpdate();
    systick_config();

    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOA);

    gd_eval_com_init(EVAL_COM1);
    //gd_eval_com_init(EVAL_COM2);

    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_13);
    gpio_bit_set(GPIOC, GPIO_PIN_13);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_8);
    gpio_bit_set(GPIOA, GPIO_PIN_8);
    uart_dma_send_init();
    int i = 0;

    pu_initialize_table();

    usart_interrupt_enable(USART0, USART_INT_RBNE);
    usart_interrupt_enable(USART0, USART_INT_ERR);

    nvic_irq_enable(USART0_IRQn, 0, 0);

    PRINT_ERROR("This GD32F303 MCU flash size is: %d\n", FMC_SIZE);
    PRINT_ERROR("Application version is: ______++++++_____1.99999++++++\n");

    init_build_time();
    i2c_interrupt_init();

    UpgradeFlag flag = {0};
    uint32_t application_crc = verify_firmware();
    if(application_crc != read_application_crc()) {
        //write_application_crc(application_crc);
        //upgrade_fmc_write_data(MUPGRADE_CRC32_ADDRESS, application_crc);
    }
    printf("Application calculate 25 block crc value is: 0x%08lX\n", application_crc);
    printf("Bootloader read application crc value is: 0x%08lX\n", read_application_crc());
    printf("The application type is: %ld\n", read_application_type());

    while (1) {
        //delay_1ms(1000);
        process_base_address(BASE_ADDRESS_1, BASE_OFFSET_1, 4);
        process_base_address(BASE_ADDRESS_2, BASE_OFFSET_2, 2);
        process_base_address(BASE_ADDRESS_3, BASE_OFFSET_3, 4);
        process_base_address(BASE_ADDRESS_4, BASE_OFFSET_4, 4);

        gpio_bit_write(GPIOA, GPIO_PIN_8, (bit_status)(1 - gpio_input_bit_get(GPIOA, GPIO_PIN_8)));
        upgrade_read_flag(&flag);
        if (flag.isupgrade == UPGRADESTART) {
            jump_to_boot();
        }
    }
    pu_free_table();

}


