#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gd32f30x.h"
#include "gd32f303e_eval.h"
#include "systick.h"
#include "pu_table.h"
#include "i2c.h"

#define MAX_PUDATA_COUNT (3)
#define MAX_FRAME_LENGTH (128)
#define USART0_DATA_ADDRESS ((uint32_t)&USART_DATA(USART0))

typedef enum {
    RECEIVING_ADDRESS,
    RECEIVING_FUNCTION,
    RECEIVING_DATA_LENGTH,
    RECEIVING_DATA,
    RECEIVING_CRC,
    RECEIVING_COMPLETE,
    RECEIVING_ERROR
} ReceivingState;

static ReceivingState current_state = RECEIVING_ADDRESS;
static CommFrame received_frame = {0};
static uint8_t received_buffer[MAX_FRAME_LENGTH] = {0};
static uint8_t received_buffer_number = 0;
static uint8_t bytes_counter = 0;
static uint16_t calc_crc = 0;
static uint8_t send_buffer[MAX_FRAME_LENGTH] = {0};

void uart_dma_send_init(void)
{
    rcu_periph_clock_enable(RCU_DMA0);
    dma_deinit(DMA0, DMA_CH3);

    dma_parameter_struct dma_init_struct = {0};
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)send_buffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = MAX_FRAME_LENGTH;
    dma_init_struct.periph_addr = USART0_DATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH3, &dma_init_struct);

    dma_circulation_disable(DMA0, DMA_CH3);
    dma_memory_to_memory_disable(DMA0, DMA_CH3);

    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
    dma_channel_enable(DMA0, DMA_CH3);
}

static void usartSendDMA(uint8_t *buffer, uint32_t size)
{
    if (dma_transfer_number_get(DMA0, DMA_CH3) == 0) {
        dma_channel_disable(DMA0, DMA_CH3);
        DMA_CHCNT(DMA0, DMA_CH3) = size;
        DMA_CHMADDR(DMA0, DMA_CH3) = (uint32_t)buffer;
        dma_channel_enable(DMA0, DMA_CH3);
    }
}

void reset_reception(void)
{
    current_state = RECEIVING_ADDRESS;
    received_frame.addressID = 0;
    received_frame.functionCode = 0;
    received_frame.dataCount = 0;
    received_frame.crcCheck = 0;
    if (received_frame.dataField) {
        free(received_frame.dataField);
        received_frame.dataField = NULL;
    }
    memset(received_buffer, 0, sizeof(received_buffer));
    received_buffer_number = 0;

    bytes_counter = 0;
    calc_crc = 0;
}

void process_upgrade(void)
{
#if 0
    UpgradeFlag flag = {0};
    flag.isupgrade = UPGRADESTART;
    upgrade_write_flag(&flag);
#endif
    upgrade_fmc_write_data(MUPGRADE_FLAG_ADDRESS, UPGRADESTART);
}

void process_read(const CommFrame *input)
{
    CommFrame *frame = createCommFrame(input->addressID, PU_FUN_READ, input->dataCount);
    if (!frame)
        return;
    uint16_t i = 0;
    for (i = 0; i < input->dataCount; i++) {
        uint32_t register_id = pu_search_id(input->dataField[i].address, input->dataField[i].offset);
        pu_set_status(register_id, 1);
        frame->dataField[i] = createBufdata(input->dataField[i].address, input->dataField[i].offset, pu_get_data(register_id));
    }
    uint16_t buf_length = 0;
    uint8_t *serialized = serializeCommFrame(frame, &buf_length);
    //memcpy(send_buffer, serialized, buf_length);
    //usartSendDMA(send_buffer, buf_length);
    if (serialized) {
        free(serialized);
        serialized = NULL;
    }
    freeCommFrame(frame);
}

void process_write(const CommFrame *input)
{
    uint16_t i = 0;
    for (i = 0; i < input->dataCount; i++) {
        uint32_t register_id = pu_search_id(input->dataField[i].address, input->dataField[i].offset);
        if (pu_get_type(register_id) == 'W') {
            pu_set_data(register_id, input->dataField[i].data);
            pu_set_status(register_id, 10);
        }
    }
}

void process_replay(void)
{
    if (received_frame.functionCode == PU_FUN_READ) {
        process_read(&received_frame);
    } else if (received_frame.functionCode == PU_FUN_WRITE) {
        process_write(&received_frame);
    } else if (received_frame.functionCode == PU_FUN_UPGRADE) {
        process_upgrade();
    }
}

void send_replay_data(uint32_t index)
{
    uint8_t replay_data[2] = {0};
    i2c_buffer_read(replay_data, pu_get_command(index), 2);
    CommFrame *frame = createCommFrame(0x01, PU_FUN_READ, 1);
    if(!frame) return;
    uint16_t voltage_value = (replay_data[1] << 8) | replay_data[0];
    pu_set_data(index, voltage_value);
    frame->dataField[0] = createBufdata(pu_get_address(index), pu_get_offset(index), pu_get_data(index));
    uint16_t buf_length = 0;
    uint8_t *serialized = serializeCommFrame(frame, &buf_length);
    memcpy(send_buffer, serialized, buf_length);
    usartSendDMA(send_buffer, buf_length);
    if (serialized) {
        free(serialized);
        serialized = NULL;
    }
    freeCommFrame(frame);
}

void write_command_data(uint32_t index)
{
    uint8_t command = pu_get_command(index);
    switch(command) {
    case 0x62:
        break;
    case 0x66:
        alarm_enable();
        break;
    }
}

void parse_received_data(uint8_t received_byte)
{
    if (current_state == RECEIVING_COMPLETE || current_state == RECEIVING_ERROR) {
        reset_reception();
    }
    switch (current_state) {
    case RECEIVING_ADDRESS: {
        if (current_state != RECEIVING_ERROR) {
            received_frame.addressID = received_byte;
            received_buffer[received_buffer_number++] = received_byte;
            current_state = RECEIVING_FUNCTION;
        }
    }
    break;

    case RECEIVING_FUNCTION: {
        if (current_state != RECEIVING_ERROR) {
            if (received_byte == PU_FUN_READ || received_byte == PU_FUN_WRITE || received_byte == PU_FUN_UPGRADE) {
                received_frame.functionCode = received_byte;
                received_buffer[received_buffer_number++] = received_byte;
                current_state = RECEIVING_DATA_LENGTH;
            } else {
                current_state = RECEIVING_ERROR;
            }
        }
    }
    break;

    case RECEIVING_DATA_LENGTH: {
        if (current_state != RECEIVING_ERROR) {
            if (bytes_counter == 0) {
                received_frame.dataCount = (uint16_t)(received_byte << 8);
                received_buffer[received_buffer_number++] = received_byte;
                bytes_counter++;
            } else if (bytes_counter == 1) {
                received_frame.dataCount |= (uint16_t)(received_byte);
                received_buffer[received_buffer_number++] = received_byte;
                bytes_counter = 0;
                if (received_frame.dataCount < 1 || received_frame.dataCount > MAX_PUDATA_COUNT) {
                    PRINT_ERROR("This length is not correct %d \n", received_frame.dataCount);
                    current_state = RECEIVING_ERROR;
                    break;
                }
                received_frame.dataField = (Bufdata *)malloc(sizeof(Bufdata) * received_frame.dataCount);
                if (!received_frame.dataField) {
                    PRINT_ERROR("This pointer is error\n");
                    current_state = RECEIVING_ERROR;
                    break;
                }
                memset(received_frame.dataField, 0, sizeof(Bufdata) * received_frame.dataCount);
                current_state = RECEIVING_DATA;
            }
        }
    }
    break;

    case RECEIVING_DATA: {
        if (current_state != RECEIVING_ERROR && received_frame.dataField) {
            uint8_t *pData = (uint8_t *)received_frame.dataField;
            pData[bytes_counter++] = received_byte;
            received_buffer[received_buffer_number++] = received_byte;
            if (bytes_counter >= (sizeof(Bufdata) * received_frame.dataCount)) {
                calc_crc = calculate_crc16(received_buffer, received_buffer_number);
                current_state = RECEIVING_CRC;
                bytes_counter = 0;
            }
        }
    }
    break;

    case RECEIVING_CRC: {
        if (bytes_counter == 0) {
            received_frame.crcCheck = (uint16_t)(received_byte << 8);
            received_buffer[received_buffer_number++] = received_byte;
            bytes_counter++;
        } else if (bytes_counter == 1) {
            received_frame.crcCheck |= received_byte;
            received_buffer[received_buffer_number++] = received_byte;
            bytes_counter = 0;
            if (calc_crc == received_frame.crcCheck) {
                current_state = RECEIVING_COMPLETE;
                int i = 0;
                int j = sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint32_t);
                uint8_t *pData = (uint8_t *)received_frame.dataField;
                for (i = 0; i < received_frame.dataCount; i++) {
                    received_frame.dataField[i].address = (pData[i * j] << 8) + pData[(i * j) + 1];
                    received_frame.dataField[i].offset = (pData[(i * j) + 2] << 8) + pData[(i * j) + 3];
                    received_frame.dataField[i].data = (pData[(i * j) + 4] << 24) + (pData[(i * j) + 5] << 16) + (pData[(i * j) + 6] << 8) + pData[(i * j) + 7];
                }
                process_replay();
            } else {
                current_state = RECEIVING_ERROR;
            }
            reset_reception();
        }
    }
    break;

    case RECEIVING_COMPLETE:
        break;

    case RECEIVING_ERROR:
        break;
    }

    if (current_state == RECEIVING_COMPLETE || current_state == RECEIVING_ERROR) {
        reset_reception();
    }
}

void USART0_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)) {
        uint8_t received_byte = (uint8_t)usart_data_receive(USART0);
        parse_received_data(received_byte);
    }
    if (usart_flag_get(USART0, USART_FLAG_ORERR)) {
        usart_flag_clear(USART0, USART_FLAG_ORERR);
    }
    if (usart_flag_get(USART0, USART_FLAG_FERR)) {
        usart_flag_clear(USART0, USART_FLAG_FERR);
    }
    if (usart_flag_get(USART0, USART_FLAG_NERR)) {
        usart_flag_clear(USART0, USART_FLAG_NERR);
    }
}
