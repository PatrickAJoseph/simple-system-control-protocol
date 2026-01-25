# simple-system-control-protocol
A simple fixed frame transport layer protocol consisting of ASCII character encoding which can be used for register like control of embedded devices. Current supported data link protocols are UART and bluetooth.

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
|    15      |        7 to 0            |    BYTE15_ENCODED         |   Takes a constant value of * indicating end of frame       |

If BYTE1_UNENCODED = 0x45, then, BYTE1_ENCODED = char(upper_nibble(BYTE1_UNENCODED)) = char(upper_nibble(0x45)) = char(0x4) = '4'
BYTE2_ENCODED = char(lower_nibble(BYTE1_UNENCODED)) = char(lower_nibble(0x45)) = char(0x5) = '5'.




