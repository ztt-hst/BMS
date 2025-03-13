#include <stdlib.h>
#include <string.h>
#include "gd32f30x.h"
#include "systick.h"
#include "gd32f303e_eval.h"
#include <stdio.h>
#include "gd32f30x_fmc.h"

#define PAGE_SIZE 2048 // 每页大小为 2KB
fmc_state_enum upgrade_fmc_write_multiple_data(uint32_t address, uint32_t *data, uint32_t length);
/* Global variables */
static const uint8_t command_start[] = {0x01, 0x5E, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0xD0, 0xCF};
static uint32_t app_address = 0x08008000;
static uint32_t backup_address = 0x08040000;

void jump_to_application(void)
{
    nvic_irq_disable(EXTI0_IRQn);
    __set_MSP(*(__IO uint32_t *)app_address);

    ((void (*)(void))(*(__IO uint32_t *)(app_address + 4)))();
}

void jump_to_backup(void)
{
    nvic_irq_disable(EXTI0_IRQn);
    __set_MSP(*(__IO uint32_t *)backup_address);

    ((void (*)(void))(*(__IO uint32_t *)(backup_address + 4)))();
}

void uint32_to_uint8_array(uint32_t input, uint8_t output[4])
{
    output[0] = (input >> 24) & 0xFF;
    output[1] = (input >> 16) & 0xFF;
    output[2] = (input >> 8) & 0xFF;
    output[3] = input & 0xFF;
}

uint32_t uint8_array_to_uint32(const uint8_t input[4])
{
    return ((uint32_t)input[0] << 24) |
           ((uint32_t)input[1] << 16) |
           ((uint32_t)input[2] << 8) |
           (uint32_t)input[3];
}

int usart_receive_with_timeout(uint8_t *data, uint32_t timeout)
{
//    uint32_t start_tick = SysTick->VAL;
    while (RESET == usart_flag_get(USART0, USART_FLAG_RBNE))
        ;
//    {
//        if ((SysTick->VAL - start_tick) >= timeout) {
//            return -1; // 超时
//        }
//    }
    *data = usart_data_receive(USART0);
    return 0;
}

static uint8_t *update_data = NULL;
static uint8_t upgrade_block_number[4];
static uint32_t total_block = 0;
uint32_t verify_firmware(void)
{
    /*initial CRC clock*/
    rcu_periph_clock_enable(RCU_CRC);
    crc_data_register_reset();
    uint32_t current_block_number = 25;
    uint32_t page_size = 2048;
    uint32_t *papp_address = (__IO uint32_t *)app_address;
    uint32_t crc_result = crc_block_data_calculate(papp_address, current_block_number * page_size);
    return crc_result;
}
void upgrade_run(void)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t result = 0;
    uint32_t page_size = MBLOCK_SIZE;

    usartSend((uint8_t *)command_start, sizeof(command_start));

    for (i = 0; i < sizeof(upgrade_block_number); i++) {
        if (usart_receive_with_timeout(&upgrade_block_number[i], 10000) != 0) {
            return;
        }
    }

    total_block = uint8_array_to_uint32(upgrade_block_number);

    uint32_to_uint8_array(total_block, upgrade_block_number);
    usartSend(upgrade_block_number, sizeof(upgrade_block_number));

    for (j = 0; j < total_block; ++j) {
        update_data = (uint8_t *)malloc(page_size);
        if (update_data == NULL) {
            return;
        }

        for (i = 0; i < page_size; ++i) {
            if (usart_receive_with_timeout(&update_data[i], 10000) != 0) {
                free(update_data);
                return;
            }
        }

        result = upgrade_fmc_write_multiple_data(app_address, (uint32_t *)update_data, page_size);
        uint32_to_uint8_array(result == FMC_READY ? app_address : result, upgrade_block_number);
        usartSend(upgrade_block_number, sizeof(upgrade_block_number));

        app_address += page_size;

        free(update_data);
        update_data = NULL;
    }
}

void upgrade_backup(void)
{
    uint32_t i = 0;
    uint32_t current_block_number = 25;
    uint32_t page_size = 2048;
    uint8_t *backup_buffer = (uint8_t *)malloc(page_size);
    if (!backup_buffer) {
        return;
    }
    uint32_t *pbackup_address = (__IO uint32_t *)app_address;
    uint8_t *psrc = (uint8_t *)pbackup_address;
    uint32_t backup_application_address = backup_address;

    for(i = 0; i < current_block_number; i++) {
        memcpy(backup_buffer, &psrc[i * page_size], page_size);
        upgrade_write_data(backup_buffer, backup_application_address);
        backup_application_address += page_size;
    }

    if(backup_buffer != NULL) {
        free(backup_buffer);
        backup_buffer = NULL;
    }
}

void upgrade_application_recover(void)
{
    uint32_t i = 0;
    uint32_t current_block_number = 25;
    uint32_t page_size = 2048;
    uint8_t *backup_buffer = (uint8_t *)malloc(page_size);
    if (!backup_buffer) {
        return;
    }
    uint32_t *pbackup_address = (__IO uint32_t *)backup_address;
    uint8_t *psrc = (uint8_t *)pbackup_address;
    uint32_t application_address = app_address;

    for(i = 0; i < current_block_number; i++) {
        memcpy(backup_buffer, &psrc[i * page_size], page_size);
        upgrade_write_data(backup_buffer, application_address);
        application_address += page_size;
    }
    if(backup_buffer != NULL) {
        free(backup_buffer);
        backup_buffer = NULL;
    }
}

/*!
    \brief      write multiple data to a specific address in flash memory
    \param[in]  address: starting address to write data
    \param[in]  data: pointer to the data to write
    \param[in]  length: length of the data to write (in bytes)
    \param[out] none
    \retval     state of FMC, refer to fmc_state_enum
*/
fmc_state_enum upgrade_fmc_write_multiple_data(uint32_t address, uint32_t *data, uint32_t length)
{
    fmc_state_enum state;
    uint32_t page_address = address & ~(PAGE_SIZE - 1); // 计算页起始地址
    uint32_t end_address = address + length;
    uint32_t current_address = address;

    // Unlock the FMC
    fmc_unlock();

    // Erase the page containing the starting address
    state = fmc_page_erase(page_address);
    if (state != FMC_READY) {
        // Handle error
        fmc_lock();
        return state;
    }

    // Write data to the specified addresses
    while (current_address < end_address) {
        state = fmc_word_program(current_address, *data++);
        if (state != FMC_READY) {
            // Handle error
            fmc_lock();
            return state;
        }
        current_address += 4; // Move to the next word
    }

    // Lock the FMC
    fmc_lock();

    return state;
}

void handle_upgrade(void)
{
    UpgradeFlag flag = {0};

    if (upgrade_read_flag(&flag) == 0 && flag.isupgrade == UPGRADESTART) {
        // 正常升级流程
        upgrade_fmc_write_data(MUPGRADE_FLAG_ADDRESS, UPGRADESTOP);

        upgrade_fmc_write_data(MUPGRADE_APPLICATION_TYPE_ADDRESS, 2000);

        upgrade_backup();

        upgrade_run();

        // 写完后改回1000，表示升级结束

        upgrade_fmc_write_data(MUPGRADE_APPLICATION_TYPE_ADDRESS, 1000);

        nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x8000);
        NVIC_SystemReset();
    }

    // 如果发现应用类型是2000，执行恢复逻辑
    if(read_application_type() == 2000) {

        upgrade_application_recover();

        upgrade_fmc_write_data(MUPGRADE_APPLICATION_TYPE_ADDRESS, 1000);

        nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x8000);
        NVIC_SystemReset();
    }

    // 设置向量表起始地址并跳转
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x8000);
    jump_to_application();
}

int main(void)
{
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0);
    __enable_irq();

    SystemInit();
    SystemCoreClockUpdate();
    systick_config();

    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOA);

    gd_eval_com_init(EVAL_COM1);


    handle_upgrade();
}
