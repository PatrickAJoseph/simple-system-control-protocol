# simple-system-control-protocol

A simple fixed frame transport layer protocol consisting of ASCII character encoding which can be used for register like control of embedded devices. Current supported data link protocols are UART and bluetooth.

# Protocol packet structure

The unencoded packet structure is as follows

|    BYTE    |    BITFIELD (0 indexed)  |      BIT FIELD NAME       |                              DESCRIPTION                    |
|------------|--------------------------|---------------------------|-------------------------------------------------------------|
|    0       |        7 to 0            |    Start of Frame         |   Takes a constant value of * indicating start of frame     |
|    1       |        7 to 3            |    Device identifier      |   A 5-bit unsigned integer indicating the device ID         |
|    1       |          2               |    Acknowledgement Bit    |   This bit is set only by the responder device              |
|    1       |          1               |     Read Operation        |   Indicates read operation on target register in initiator  |
|    1       |          0               |     Write Operation       |   Indicates write operation on target register in initiator |
|    2       |        7 to 0            |     Register number       |   Target register number to be operated on                  |
|    3       |        7 to 0            |     Register Byte 3       |   Byte 3 of the 32-bit register value                       |
|    4       |        7 to 0            |     Register Byte 2       |   Byte 2 of the 32-bit register value                       |
|    5       |        7 to 0            |     Register Byte 1       |   Byte 1 of the 32-bit register value                       |
|    6       |        7 to 0            |     Register Byte 0       |   Byte 0 of the 32-bit register value                       |
|    7       |        7 to 0            |       CRC-8               |   CRC-8 CCITT (https://www.3dbrew.org/wiki/CRC-8-CCITT)     |
|            |                          |                           |   CRC-8 is calculated only for BYTE1 to BYTE6               |
|    8       |        7 to 0            |     End of frame          |   Takes a constant value of # indicating end of frame       |

Before transmission of an unencoded message frame, the unencoded message is encoded into the following format. Alll bytes except the start of frame
and the end of frame characters are encoded. Note that BYTEx_UNENCODED represents the `x`th byte of the unencoded packet format shown in the previous
table.

|    BYTE    |    BITFIELD (0 indexed)  |      BIT FIELD NAME       |                              DESCRIPTION                    |
|------------|--------------------------|---------------------------|-------------------------------------------------------------|
|    0       |        7 to 0            |    Start of Frame         |   Takes a constant value of * indicating start of frame     |
|    1       |        7 to 0            |    BYTE1_ENCODED          |   Character-encoded upper nibble of BYTE1_UNENCODED         |
|    2       |        7 to 0            |    BYTE2_ENCODED          |   Character-encoded lower nibble of BYTE1_UNENCODED         |
|    3       |        7 to 0            |    BYTE3_ENCODED          |   Character-encoded upper nibble of BYTE2_UNENCODED         |
|    4       |        7 to 0            |    BYTE4_ENCODED          |   Character-encoded lower nibble of BYTE2_UNENCODED         |
|    5       |        7 to 0            |    BYTE5_ENCODED          |   Character-encoded upper nibble of BYTE3_UNENCODED         |
|    6       |        7 to 0            |    BYTE6_ENCODED          |   Character-encoded lower nibble of BYTE3_UNENCODED         |
|    7       |        7 to 0            |    BYTE7_ENCODED          |   Character-encoded upper nibble of BYTE4_UNENCODED         |
|    8       |        7 to 0            |    BYTE8_ENCODED          |   Character-encoded lower nibble of BYTE4_UNENCODED         |
|    9       |        7 to 0            |    BYTE9_ENCODED          |   Character-encoded upper nibble of BYTE5_UNENCODED         |
|    10      |        7 to 0            |    BYTE10_ENCODED         |   Character-encoded lower nibble of BYTE5_UNENCODED         |
|    11      |        7 to 0            |    BYTE11_ENCODED         |   Character-encoded upper nibble of BYTE6_UNENCODED         |
|    12      |        7 to 0            |    BYTE12_ENCODED         |   Character-encoded lower nibble of BYTE6_UNENCODED         |
|    13      |        7 to 0            |    BYTE13_ENCODED         |   Character-encoded upper nibble of BYTE7_UNENCODED         |
|    14      |        7 to 0            |    BYTE14_ENCODED         |   Character-encoded lower nibble of BYTE7_UNENCODED         |
|    15      |        7 to 0            |    BYTE15_ENCODED         |   Takes a constant value of # indicating end of frame       |

If BYTE1_UNENCODED = 0x45, then, BYTE1_ENCODED = char(upper_nibble(BYTE1_UNENCODED)) = char(upper_nibble(0x45)) = char(0x4) = '4'
BYTE2_ENCODED = char(lower_nibble(BYTE1_UNENCODED)) = char(lower_nibble(0x45)) = char(0x5) = '5'.

The following steps shows the encoding and decoding of a packet exchanged between an initiator and a responder.
Consider a initiator sending a packet to a responder with device ID 4 with a read request for register 0x10.

1. Form the unencoded packet from the sender side.

|    BYTE    |    BITFIELD (0 indexed)  |      BIT FIELD NAME       |                        VALUE                                |
|------------|--------------------------|---------------------------|-------------------------------------------------------------|
|    0       |        7 to 0            |    Start of Frame         |                      '*' = 0x2a                             |
|    1       |        7 to 3            |    Device identifier      |                         0x04                                |
|    1       |          2               |    Acknowledgement Bit    |   0x00 (must be always set to zero by the initiator)        |
|    1       |          1               |     Read Operation        |                         0x01                                |
|    1       |          0               |     Write Operation       |                         0x00                                |
|    1       |        7 to 0            |     Net register value    |   (0x04 << 3) | ( 0x00 << 2) | ( 0x01 << 1 ) | 0x00 = 0x22  | 
|    2       |        7 to 0            |     Register number       |                         0x10                                |
|    3       |        7 to 0            |     Register Byte 3       |   0x00   (for read operations, register byte must be zero ) |
|    4       |        7 to 0            |     Register Byte 2       |   0x00   (for read operations, register byte must be zero ) |
|    5       |        7 to 0            |     Register Byte 1       |   0x00   (for read operations, register byte must be zero ) |
|    6       |        7 to 0            |     Register Byte 0       |   0x00   (for read operations, register byte must be zero ) |
|    7       |        7 to 0            |       CRC-8               |   CRC8_CCITT(0x22,0x10,0x00,0x00,0x00,0x00) = 0x5B          |
|    8       |        7 to 0            |     End of frame          |                     '#' = 0x23                              |

2. Form the encoded packet.

|    BYTE    |    BITFIELD (0 indexed)  |      BIT FIELD NAME       |                           VALUE                             |
|------------|--------------------------|---------------------------|-------------------------------------------------------------|
|    0       |        7 to 0            |    Start of Frame         |                           0x2a                              |
|    1       |        7 to 0            |    BYTE1_ENCODED          |   char(upper_nibble(BYTE1_UNENCODED)) = '2' = 0x32          |
|    2       |        7 to 0            |    BYTE2_ENCODED          |   char(lower_nibble(BYTE1_UNENCODED)) = '2' = 0x32          |
|    3       |        7 to 0            |    BYTE3_ENCODED          |   char(upper_nibble(BYTE2_UNENCODED)) = '1' = 0x31          |
|    4       |        7 to 0            |    BYTE4_ENCODED          |   char(lower_nibble(BYTE2_UNENCODED)) = '0' = 0x30          |
|    5       |        7 to 0            |    BYTE5_ENCODED          |   char(upper_nibble(BYTE3_UNENCODED)) = '0' = 0x30          |
|    6       |        7 to 0            |    BYTE6_ENCODED          |   char(lower_nibble(BYTE3_UNENCODED)) = '0' = 0x30          |
|    7       |        7 to 0            |    BYTE7_ENCODED          |   char(upper_nibble(BYTE4_UNENCODED)) = '0' = 0x30          |
|    8       |        7 to 0            |    BYTE8_ENCODED          |   char(lower_nibble(BYTE4_UNENCODED)) = '0' = 0x30          |
|    9       |        7 to 0            |    BYTE9_ENCODED          |   char(upper_nibble(BYTE5_UNENCODED)) = '0' = 0x30          |
|    10      |        7 to 0            |    BYTE10_ENCODED         |   char(lower_nibble(BYTE5_UNENCODED)) = '0' = 0x30          |
|    11      |        7 to 0            |    BYTE11_ENCODED         |   char(upper_nibble(BYTE6_UNENCODED)) = '0' = 0x30          |
|    12      |        7 to 0            |    BYTE12_ENCODED         |   char(lower_nibble(BYTE6_UNENCODED)) = '0' = 0x30          |
|    13      |        7 to 0            |    BYTE13_ENCODED         |   char(upper_nibble(BYTE7_UNENCODED)) = '5' = 0x35          |
|    14      |        7 to 0            |    BYTE14_ENCODED         |   char(lower_nibble(BYTE7_UNENCODED)) = 'B' = 0x42          |
|    15      |        7 to 0            |    BYTE15_ENCODED         |                         0x23                                |

3. Assume that responder for device 4 has 32-bit value 0x4519AE50 for register 0x10. The reponder constructs the following unencoded
   response packet first.

|    BYTE    |    BITFIELD (0 indexed)  |      BIT FIELD NAME       |                        VALUE                                |
|------------|--------------------------|---------------------------|-------------------------------------------------------------|
|    0       |        7 to 0            |    Start of Frame         |                      '*' = 0x2a                             |
|    1       |        7 to 3            |    Device identifier      |                         0x04                                |
|    1       |          2               |    Acknowledgement Bit    |   0x01 (must be always set to one by the responder)         |
|    1       |          1               |     Read Operation        |                         0x01                                |
|    1       |          0               |     Write Operation       |                         0x00                                |
|    1       |        7 to 0            |     Net register value    |   (0x04 << 3) | ( 0x01 << 2) | ( 0x01 << 1 ) | 0x00 = 0x26  | 
|    2       |        7 to 0            |     Register number       |                         0x10                                |
|    3       |        7 to 0            |     Register Byte 3       |                         0x45                                |
|    4       |        7 to 0            |     Register Byte 2       |                         0x19                                |
|    5       |        7 to 0            |     Register Byte 1       |                         0xAE                                |
|    6       |        7 to 0            |     Register Byte 0       |                         0x50                                |
|    7       |        7 to 0            |       CRC-8               |   CRC8_CCITT(0x26,0x10,0x45,0x19,0xAE,0x50) = 0xCB          |
|    8       |        7 to 0            |     End of frame          |                     '#' = 0x23                              |

4. The unencoded packet is then encoded by the responder and transmitted back to the initiator.

|    BYTE    |    BITFIELD (0 indexed)  |      BIT FIELD NAME       |                           VALUE                             |
|------------|--------------------------|---------------------------|-------------------------------------------------------------|
|    0       |        7 to 0            |    Start of Frame         |                           0x2a                              |
|    1       |        7 to 0            |    BYTE1_ENCODED          |   char(upper_nibble(BYTE1_UNENCODED)) = '2' = 0x32          |
|    2       |        7 to 0            |    BYTE2_ENCODED          |   char(lower_nibble(BYTE1_UNENCODED)) = '6' = 0x36          |
|    3       |        7 to 0            |    BYTE3_ENCODED          |   char(upper_nibble(BYTE2_UNENCODED)) = '1' = 0x31          |
|    4       |        7 to 0            |    BYTE4_ENCODED          |   char(lower_nibble(BYTE2_UNENCODED)) = '0' = 0x30          |
|    5       |        7 to 0            |    BYTE5_ENCODED          |   char(upper_nibble(BYTE3_UNENCODED)) = '4' = 0x34          |
|    6       |        7 to 0            |    BYTE6_ENCODED          |   char(lower_nibble(BYTE3_UNENCODED)) = '5' = 0x35          |
|    7       |        7 to 0            |    BYTE7_ENCODED          |   char(upper_nibble(BYTE4_UNENCODED)) = '1' = 0x31          |
|    8       |        7 to 0            |    BYTE8_ENCODED          |   char(lower_nibble(BYTE4_UNENCODED)) = '9' = 0x39          |
|    9       |        7 to 0            |    BYTE9_ENCODED          |   char(upper_nibble(BYTE5_UNENCODED)) = 'A' = 0x41          |
|    10      |        7 to 0            |    BYTE10_ENCODED         |   char(lower_nibble(BYTE5_UNENCODED)) = 'E' = 0x45          |
|    11      |        7 to 0            |    BYTE11_ENCODED         |   char(upper_nibble(BYTE6_UNENCODED)) = '5' = 0x35          |
|    12      |        7 to 0            |    BYTE12_ENCODED         |   char(lower_nibble(BYTE6_UNENCODED)) = '0' = 0x30          |
|    13      |        7 to 0            |    BYTE13_ENCODED         |   char(upper_nibble(BYTE7_UNENCODED)) = 'C' = 0x43          |
|    14      |        7 to 0            |    BYTE14_ENCODED         |   char(lower_nibble(BYTE7_UNENCODED)) = 'B' = 0x42          |
|    15      |        7 to 0            |    BYTE15_ENCODED         |                         0x23                                |

# Architecture diagram of protocol stack

The following diagram shows the architecture diagram of the communication protocol.

<img width="900" height="700" alt="image" src="https://github.com/user-attachments/assets/29eaa13e-f505-45be-bf13-56383fafad3e" />

# Supported languages

The simple system control protocol (herein referred to as SCCP) supports the following programming languages

* Node.js (initiator only)
* Python (initiator + responder)
* C (responder stack only)
* LabVIEW (initiator only)

# Storage of register bitfield information

All bitfields of registers including their description will be documented in a YAML file making maintenance and readability easier.
A python script `sccp_utils_yaml2c.py` can be used to convert the YAML file into a C header file which can then be used by the
C application requiring SCCP support.

# Current supported data link protocols

The simple system control protocol (herein referred to as SSCP) supports the following data link protocols (eventhough some of them
are not technically data link protocols)

* Socket (TCP/IP)
* UART (RS-485 interface)
* BLE (one 16-byte string characteristics for Initiator, one 16-byte string character for Responder)

## Socket

The use of socket connections enables the control of an SSCP supported device via the local network.

Given a base port address M and a device ID N, two ports are opened for connection on the local host / target IP address
The initiator uses has the port number `M + 2*N` and the responder uses the port number `M + 2*N + 1`. The initiator sends
over messages to the responder over port number `M + 2*N` and can read back the response from the responder over port number
`M + 2*N + 1`. The responder receives a packet over port number `M + 2*N` and sends over its response over `M + 2*N + 1`.

## UART / RS-485 interface

The use of RS-485 interface allows upto 32 devices to be connected on a single RS-485 bus and to be controlled by a single host
device. This ensures that devices using the SCCP protocol stack can be controlled by a single initiator/master.

## Bluetooth / BLE

The SSCP protocol mandates that the SCCP BLE service UUID has the following format.

|           Byte index (0-indexed)           |                 Description                |
|--------------------------------------------|--------------------------------------------|
|                 0                          |                    0x40                    |
|                 1                          |                 Device ID                  |
|                 2                          |              Base port number (lower)      |
|                 3                          |              Base port number (upper)      |
|               4 - 15                       |                    0xFF                    |

The base port number is used for connecting a BLE device to another application via the socket interface.

The SSCP bluetooth service consists of the following characteristics

1. Initiator write / Responder read (IWRR) characteristic
   The initiator write characteristic is a 16-byte string. The initiator (host PC consisting of GATT client) writes a 16-byte
   ASCII encoded packet. The responder has to register a callback in the GATT server associated with this initiator write
   operation. The IWRR characteristic is a read characteristic from the perspective of a GATT server.

   The following table shows the structure of the IWRR characteristic UUID

   |           Byte index (0-indexed)           |                 Description                |
   |--------------------------------------------|--------------------------------------------|
   |                 0                          |                    0x41                    |
   |                 1                          |                 Device ID                  |
   |                 2                          |              Base port number (lower)      |
   |                 3                          |              Base port number (upper)      |
   |               4 - 15                       |                    0xFF                    |


2. Initiator read / Responder write (IRRW) characteristics
   Once the responder processes the request from the initiator, it has to write a 16-byte string into the IRRW characteristic.
   The initiator has to register a callback function to capture a write event to the IRRW characteristic. The IWRR characteristic
   is a write characteristic from the perspective of a GATT server.

   The following table shows the structure of the IRRW characteristic UUID

   |           Byte index (0-indexed)           |                 Description                |
   |--------------------------------------------|--------------------------------------------|
   |                 0                          |                    0x42                    |
   |                 1                          |                 Device ID                  |
   |                 2                          |              Base port number (lower)      |
   |                 3                          |              Base port number (upper)      |
   |               4 - 15                       |                    0xFF                    |
