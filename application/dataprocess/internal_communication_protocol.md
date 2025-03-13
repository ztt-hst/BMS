## **Serial Communication Protocol between ESP32 and MCU V2.2**

The Modbus protocol uses a master-slave relationship for communication. In this setup, the controller (EPS32) acts as the master, and the Battery Management System (BMS) acts as the slave. Communication always starts with the controller. The controller sends requests to the BMS, specifying the BMS's unique address. The BMS only responds to these requests and never initiates communication itself. Because the BMS only speaks when spoken to, it doesn't occupy the communication bus unnecessarily. This prevents data conflicts on the bus.

Physical Interface

| Communication Interface | UART       |
| ----------------------- | ---------- |
| Baud Rate               | 115200 bps |
| Data Bits               | 8          |
| Stop Bits               | 1          |
| Parity Bit              | N          |

Protocol Format

The information is transmitted asynchronously using **hexadecimal** communication, with the following frame format.

| Address Code | Function Code | Data Field | CRC Check |
| ------------ | ------------- | ---------- | --------- |
| 1 Byte       | 1 Byte        | n Bytes    | 2 Bytes   |

The **address code**, the first byte of each communication frame, ranges from 0x01 to 0xF0. Each BMS on the bus must have a unique address, and only the BMS with an address matching the ESP32's request will respond with data.

The **function code**, the second byte of each communication frame, specifies the operation the BMS device should perform, as detailed below.

| Function | Definition     | Operation                                |
| -------- | -------------- | ---------------------------------------- |
| 0xC0     | Read Register  | Read the data from one or more registers |
| 0x03     | Write Register | Write data to one or more registers      |
| 0x5E     | Upgrade        | BMS upgrade command                      |

The **data field** varies with the function code and data direction, comprising combinations such as:"data length + register address + [register value]" "register address + operation data" "register address + number of registers to operate + data length + data" Detailed explanations for each function code's data field are provided in the "Function Code Analysis" section.

During communication between the controller (ESP32) and the BMS, the Cyclic Redundancy Check (CRC) ensures data integrity. The ESP32 sends commands to the BMS, and only the BMS with the correct address will process them. Upon receiving a command, the addressed BMS performs a CRC check. If the check passes, the BMS executes the command and returns a response containing the address code, function code, result data, and a new CRC checksum. If the address is incorrect or the CRC check fails, the BMS remains silent and does not respond.

Example:PUT: 01 C0 02   1202    53B9 “Read the battery voltage of one cell with ID 0x01 and address 0x1202.”

GET: 01 C0 1202 0C80 3442

"the voltage of the cell with ID 0x01 and address 0x1202 is 3200mV."

Upgrade Example:

PUT: 01 5E 8008 Firmware/PAGE CRC

GET: 01 5E 8008 CRC

PUT: 01 5E 800C Firmware/PAGE CRC

GET: 01 5E 800C CRC

......Send firmware with page size and index++

\#define POLYNOMIAL (0x11021) //Calculate the CRC-16-CCITT (XMODEM)

uint16_t calculate_crc16(const uint8_t *data, size_t length) {

​    uint16_t crc = 0xFFFF;

​    size_t i, j;

​        for (i = 0; i < length; i++) {

​        crc ^= (data[i] << 8);

​             for (j = 8; j > 0; j--) {

​            if (crc & 0x8000) {

​                crc = (crc << 1) ^ POLYNOMIAL; 

​            } else {

​                crc <<= 1;

​            }

​        }

​    }

​    return crc & 0xFFFF;

}