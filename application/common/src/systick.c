/*!
    \file  systick.c
    \brief the systick configuration file

    \version 2017-02-10, V1.0.0, firmware for GD32F30x
    \version 2018-10-10, V1.1.0, firmware for GD32F30x
    \version 2018-12-25, V2.0.0, firmware for GD32F30x
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32f30x.h"
#include "systick.h"
volatile static uint32_t delay;

uint32_t read_application_type(void)
{
    uint32_t address = MUPGRADE_APPLICATION_TYPE_ADDRESS;
    return *(uint32_t *)(address);
}

uint32_t read_application_crc(void)
{
    uint32_t address = MUPGRADE_CRC32_ADDRESS;
    return *(uint32_t *)(address);
}

int upgrade_read_flag(UpgradeFlag *pflag)
{
    uint32_t address = MUPGRADE_FLAG_ADDRESS;
    pflag->isupgrade = *(int *)(address);
    return 0;
}

fmc_state_enum upgrade_fmc_write_data(uint32_t address, uint32_t data)
{
#if 0
#define PAGE_SIZE 2048
    static uint32_t current_page_address = 0xFFFFFFFF; // 当前页地址
    fmc_state_enum state;
    uint32_t page_address = address & ~(PAGE_SIZE - 1); // 计算页起始地址

    // Unlock the FMC
    fmc_unlock();

    // 如果当前页地址与目标地址所在页不同，则擦除目标地址所在页
    if (current_page_address != page_address) {
        current_page_address = page_address; // 更新当前页地址
        state = fmc_page_erase(page_address);
        if (state != FMC_READY) {
            // Handle error
            fmc_lock();
            return state;
        }
    }
#else
    fmc_state_enum state;
    fmc_unlock();
    state = fmc_page_erase(address);
    if (state != FMC_READY) {
        // Handle error
        fmc_lock();
        return state;
    }
#endif
    // Write data to the specified address
    state = fmc_word_program(address, data);
    if (state != FMC_READY) {
        // Handle error
        fmc_lock();
        return state;
    }

    // Lock the FMC
    fmc_lock();

    return state;
}

int upgrade_write_data(uint8_t *pbuf, uint32_t address)
{
    uint32_t page_size = MBLOCK_SIZE;
    uint32_t start_page = address;
    uint32_t *ptrs = (uint32_t *)pbuf;
    uint32_t word_count = page_size / 4U;
    fmc_state_enum fmc_state = FMC_TOERR;

    uint32_t i = 0;

    fmc_unlock();

    fmc_state = fmc_page_erase(address);
    if (FMC_READY != fmc_state) {
        fmc_lock();
        return fmc_state;
    }
    for (i = 0; i < word_count; ++i) {
        fmc_state = fmc_word_program(start_page, *ptrs++);
        if (FMC_READY != fmc_state) {
            fmc_lock();
            return fmc_state;
        }
        start_page += 4U;
    }

    fmc_lock();

    return 0;
}

void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U)) {
        /* capture error */
        while (1) {
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

/*!
    \brief      delay a time in milliseconds
    \param[in]  count: count in milliseconds
    \param[out] none
    \retval     none
*/
void delay_1ms(uint32_t count)
{
    delay = count;

    while (0U != delay) {
    }
}

/*!
    \brief      delay decrement
    \param[in]  none
    \param[out] none
    \retval     none
*/
void delay_decrement(void)
{
    if (0U != delay) {
        delay--;
    }
}

void jump_to_boot(void)
{
    nvic_irq_disable(EXTI0_IRQn);
    __set_MSP(*(__IO uint32_t *)FLASH_BASE);

    ((void (*)(void))(*(__IO uint32_t *)(FLASH_BASE + 4)))();
}

void delay_1us(uint32_t us)
{
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    uint32_t start = SysTick->VAL;

    while ((start - SysTick->VAL) < ticks) {
        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
            start += SysTick->LOAD;
        }
    }
}