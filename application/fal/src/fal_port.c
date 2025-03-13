#include <string.h>
#include <fal.h>
#include <gd32f30x.h>
#include "systick.h"

static int init(void)
{
    return 1;
}

static int ef_err_port_cnt = 0;
int on_ic_read_cnt = 0;
int on_ic_write_cnt = 0;

void feed_dog(void)
{
}

static int read(long offset, uint8_t *buf, size_t size)
{
    size_t i;
    uint32_t addr = gd32f303re_flash.addr + offset;

    if (addr % 4 != 0)
        ef_err_port_cnt++;

    for (i = 0; i < size; i++, addr++, buf++) {
        *buf = *(uint8_t *)addr;
    }
    on_ic_read_cnt++;
    return size;
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    size_t i;
    uint32_t addr = gd32f303re_flash.addr + offset;
    fmc_state_enum flash_status = FMC_TOERR;

    uint32_t write_data __attribute__ ((aligned (4)));
    uint32_t read_data  __attribute__ ((aligned (4)));

    if (addr % 4 != 0)
        ef_err_port_cnt++;

    fmc_unlock();
    for (i = 0; i < size; i += 4, buf += 4, addr += 4) {

        memcpy(&write_data, buf, 4);

        flash_status = fmc_word_program(addr, write_data);
        if (flash_status != FMC_READY) {
            fmc_lock();
            return -1;
        }
        read_data = *(uint32_t *)addr;
        /* You can add your code under here. */
        if (read_data != write_data) {
            fmc_lock();
            return -1;
        } else {
            feed_dog();
        }
    }
    fmc_lock();

    on_ic_write_cnt++;
    return size;
}

static int erase(long offset, size_t size)
{
    uint32_t addr = gd32f303re_flash.addr + offset;

    fmc_state_enum flash_status = FMC_TOERR;
    size_t erase_pages, i;

    erase_pages = size / MBLOCK_SIZE;
    if (size % MBLOCK_SIZE != 0) {
        erase_pages++;
    }

    fmc_unlock();
    for (i = 0; i < erase_pages; i++) {
        flash_status = fmc_page_erase(addr + (MBLOCK_SIZE * i));
        if (flash_status != FMC_READY) {
            fmc_lock();
            return -1;
        } else {
            feed_dog();
        }
    }
    fmc_lock();

    return size;
}

const struct fal_flash_dev gd32f303re_flash = {
    .name = "gd32f303re_flash",
    .addr = 0x08040000,
    .len = 252 * 1024,
    .blk_size = 2 * 1024,
    .ops = {init, read, write, erase},
    .write_gran = 32
};