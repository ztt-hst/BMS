#ifndef PU_TABLE_H
#define PU_TABLE_H

#include <stdint.h>

typedef struct __attribute__((packed))
{
    uint16_t address;
    uint16_t offset;
    uint32_t data;
}
Bufdata;

typedef struct __attribute__((packed))
{
    uint8_t addressID;
    uint8_t functionCode;
    uint16_t dataCount;
    Bufdata *dataField;
    uint16_t crcCheck;
}
CommFrame;

#define PU_FUN_READ     (0xC0)
#define PU_FUN_WRITE    (0x03)
#define PU_FUN_UPGRADE  (0x5E)

void pu_initialize_table(void);

void pu_free_table(void);

uint32_t pu_search_id(uint32_t address, uint32_t offset);

uint32_t pu_get_address(uint32_t index);

uint32_t pu_get_offset(uint32_t index);

uint32_t pu_get_status(uint32_t index);

void pu_set_status(uint32_t index, uint32_t status);

char pu_get_type(uint32_t index);

uint32_t pu_get_length(uint32_t index);

uint32_t pu_get_data(uint32_t index);

uint8_t pu_get_command(uint32_t index);

void pu_set_data(uint32_t index, uint32_t data);

uint16_t calculate_crc16(const uint8_t *data, uint16_t length);

Bufdata createBufdata(uint16_t address, uint16_t offset, uint32_t data);

CommFrame *createCommFrame(uint8_t addressID, uint8_t functionCode, uint16_t dataCount);

uint8_t *serializeCommFrame(CommFrame *frame, uint16_t *data_size);

void freeCommFrame(CommFrame *frame);

CommFrame* copy_received_frame(const CommFrame *frame);

void free_copied_frame(CommFrame *frame);

#endif // PU_TABLE_H