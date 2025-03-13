#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#define FAL_PART_HAS_TABLE_CFG
#define FLASH_DEV_NAME "gd32f303re_flash"

extern const struct fal_flash_dev gd32f303re_flash;

/* flash device table */
#define FAL_FLASH_DEV_TABLE \
    {                       \
        &gd32f303re_flash,  \
    }

#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                                 \
    {                                                                                  \
        {FAL_PART_MAGIC_WORD, "fdb_kvdb1", FLASH_DEV_NAME, 0, 126 * 1024, 0},          \
        {FAL_PART_MAGIC_WORD, "fdb_tsdb1", FLASH_DEV_NAME, 126 * 1024, 126 * 1024, 0}, \
    }
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */