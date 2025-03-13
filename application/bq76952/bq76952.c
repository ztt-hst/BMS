#include <stdio.h>
#include "i2c.h"

void get_16_cell_voltage(void)
{
    uint8_t command = 0x14;
    uint8_t j = 0x00;
    uint8_t read_command[2] = {0};
    uint16_t voltage_value = 0;
    for(command = 0x14, j = 0x00; command < 0x34; command += 2, j += 2) {
        delay_1ms(10);
        if(i2c_buffer_read(read_command, command, 2) == 1) {
            voltage_value = (read_command[1] << 8) | read_command[0];
            pu_set_data(pu_search_id(0x1200, j), voltage_value);
        } else {
            pu_set_data(pu_search_id(0x1200, j), 0);
            //printf("Not read voltage value correctly\n");
        }
    }
}

void get_all_temperature(void)
{
    uint8_t read_command[2] = {0};
    uint16_t voltage_value = 0;
    uint8_t command = 0x68;
    uint16_t j = 0x00F2;
    for( command = 0x68; command < 0x7c; command += 2, j += 2) {
        delay_1ms(10);
        if(i2c_buffer_read(read_command, command, 2) == 1) {
            voltage_value = (read_command[1] << 8) | read_command[0];
            pu_set_data(pu_search_id(0x1200, j), voltage_value);
        } else {
            pu_set_data(pu_search_id(0x1200, j), 0);
            //printf("Not read temperature correctly\n");
        }
    }
}

void get_current(void)
{
    uint8_t read_command[2] = {0};
    uint16_t value = 0;
    uint8_t command = 0x3A;
    uint8_t id = 0x98;
    if(i2c_buffer_read(read_command, command, 2) == 1) {
        value = (read_command[1] << 8) | read_command[0];
        pu_set_data(pu_search_id(0x1200, id), value);
    } else {
        pu_set_data(pu_search_id(0x1200, id), 0);
        //printf("Not read this current correctly\n");
    }
}

void alarm_enable(void)
{
    uint8_t command = 0x66;
    uint8_t write_command[2] = {0x82, 0xF0};
    if(i2c_buffer_write(write_command, command, 2) != 1) {
        printf("alarm enable not success\n");
    }
}

void get_afe_device_number(void)
{
    uint8_t subcommand[2] = {0x01, 0x00};
    uint8_t replay_data[2] = {0};
    uint16_t value = 0;
    if(i2c_buffer_write(subcommand, 0x3E, 2) == 1) {
        delay_1ms(2);
    } else {
        printf("write afe subcommand value error \n");
    }
    if(i2c_buffer_read(replay_data, 0x40, 2) == 1) {
        value = (replay_data[1] << 8) | replay_data[0];
        pu_set_data(pu_search_id(0x1400, 0x28), value);
    } else {
        printf("read afe manufacturer value error \n");
    }
}

void get_afe_manufacture_status(void)
{
    uint8_t subcommand[2] = {0x57, 0x00};
    uint8_t replay_data[2] = {0};
    uint16_t value = 0;
    if(i2c_buffer_write(subcommand, 0x3E, 2) == 1) {
        delay_1ms(2);
    } else {
        printf("write afe manufacture status error\n");
    }
    if(i2c_buffer_read(replay_data, 0x40, 2) == 1) {
        value = (replay_data[1] << 8) | replay_data[0];
        put_set_data(pu_search_id(0x1400, 0x2C), value);
    } else {
        printf("read afe manufacture status error\n");
    }
}

void enable_afe_fet(void)
{
    uint8_t subcommand[2] = {0x22, 0x00};
    if(i2c_buffer_write(subcommand, 0x3E, 2) != 1) {
        printf("enable afe fet status error\n");
    }
}

void afe_reset(void)
{
    uint8_t subcommand[2] = {0x12, 0x00};
    if(i2c_buffer_write(subcommand, 0x3E, 2) != 1) {
        printf("reset afe bq76952 chip error\n");
    }
}

void get_over_voltage(void)
{
    uint8_t subcommand[2] = {0x61, 0x92};
    uint8_t replay_data[2] = {0};
    uint16_t value = 0;
    if(i2c_buffer_write(subcommand, 0x3E, 2) == 1) {
        delay_1ms(2);
    } else {
        printf("send get over voltage value error\n");
    }
    if(i2c_buffer_read(replay_data, 0x40, 2) == 1) {
        value = (replay_data[1] << 8) | replay_data[0];
        put_set_data(pu_search_id(0x1000, 0x10), value);
    } else {
        printf("read over voltage value error\n");
    }
}

void get_hardware_version(void)
{
    uint8_t subcommand[2] = {0x03, 0x00};
    uint8_t replay_data[2] = {0};
    uint16_t value = 0;
    if(i2c_buffer_write(subcommand, 0x3E, 2) == 1) {
        delay_1ms(2);
    } else {
        printf("send hardware version command error\n");
    }
    if(i2c_buffer_read(replay_data, 0x40, 2) == 1) {
        value = (replay_data[1] << 8) | replay_data[0];
        pu_set_data(pu_search_id(0x1400, 0x30), value);
    } else {
        printf("get hardware version command error\n");
    }
}

void afe_turn_off_balance(void)
{
    uint8_t subcommand[0] = {0x00, 0x00};
    if(i2c_buffer_write(subcommand, 0x3E, 2) == 1) {
        delay_1ms(1000);
    } else {
        printf("Turn off the balance command error\n");
    }
}

