/*!
    \file  systick.h
    \brief the header file of systick

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

#ifndef SYS_TICK_H
#define SYS_TICK_H

#include <stdint.h>

#ifdef APP_START_ADDRESS
#define MAPP_START_ADDRESS ((uint32_t)(APP_START_ADDRESS))
#else
#error "APP_START_ADDRESS is not defined. Please define it in the Makefile."
#endif

#ifdef UPGRADE_FLAG_ADDRESS
#define MUPGRADE_FLAG_ADDRESS ((uint32_t)(UPGRADE_FLAG_ADDRESS))
#else
#error "UPGRADE_FLAG_ADDRESS is not defined. Please define it in the Makefile."
#endif

#ifdef BLOCK_SIZE
#define MBLOCK_SIZE ((uint32_t)(BLOCK_SIZE))
#else
#error "BLOCK_SIZE is not defined. Please define it in the Makefile."
#endif

#ifdef BACK_APP_LINK_ADDRESS
#define MBACK_APP_LINK_ADDRESS ((uint32_t)(BACK_APP_LINK_ADDRESS))
#else
#error "BACK_APP_LINK_ADDRESS is not defined. Please define it in the Makefile."
#endif

#ifdef UPGRADE_CRC32_ADDRESS
#define MUPGRADE_CRC32_ADDRESS ((uint32_t)(UPGRADE_CRC32_ADDRESS))
#else
#error "UPGRADE_CRC32_ADDRESS is not defined. Please define it in the Makefile."
#endif

#ifdef UPGRADE_APPLICATION_TYPE_ADDRESS
#define MUPGRADE_APPLICATION_TYPE_ADDRESS ((uint32_t)(UPGRADE_APPLICATION_TYPE_ADDRESS))
#else
#error "UPGRADE_APPLICATION_TYPE_ADDRESS is not defined. Please define it in the Makefile."
#endif

typedef struct upgradeflag {
    int isupgrade;
} UpgradeFlag;

#define UPGRADESTART (0x1000)
#define UPGRADESTOP (0x2000)

int write_application_type(const uint32_t input);
uint32_t read_application_type(void);

int write_application_crc(const uint32_t input);
uint32_t read_application_crc(void);

/* write upgrade flag into flash */
int upgrade_write_flag(const UpgradeFlag *pflag);

/* read upgrade flag from flash */
int upgrade_read_flag(UpgradeFlag *pflag);

fmc_state_enum upgrade_fmc_write_data(uint32_t address, uint32_t data);

/* write upgrade firmware into flash */
int upgrade_write_data(uint8_t *pbuf, uint32_t address);

/* configure systick */
void systick_config(void);

/* delay a time in milliseconds */
void delay_1ms(uint32_t count);

/* delay decrement */
void delay_decrement(void);

void jump_to_boot(void);

void delay_1us(uint32_t us);
#endif /* SYS_TICK_H */
