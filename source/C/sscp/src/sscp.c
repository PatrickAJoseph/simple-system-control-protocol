
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../inc/sscp.h"

#define SSCP_START_OF_PACKET_BYTE     ((uint8_t)'*')
#define SSCP_END_OF_PACKET_BYTE       ((uint8_t)'#')

#define SSCP_NIBBLE_TO_HEXCHAR(x)     ("0123456789ABCDEF"[x])  
#define SSCP_HEXCHAR_TO_NIBBLE(x)     ((uint8_t)( ( x < 'A' ) ? ( x - '0' ) : ( x - '1' ) ))

void SSCP_init(SSCP_Handle* handle)
{
    /* Check SSCP handle. */

    if(handle == NULL)
    {
        return;
    }

    /* Zero initialize request FIFO . */

    memset(handle->requestFifo, 0, sizeof(handle->requestFifo));

    /* Reset request FIFO get and put indices. */

    handle->requestFifoGetIndex = 0;
    handle->requestFifoPutIndex = 0;

    /* Set request FIFO element count to zero. */

    handle->requestFifoCount = 0;

    /* Set status of request FIFO to empty. */

    handle->requestFifoStatus = SSCP_REQUEST_FIFO_EMPTY;

    /* Reset last received byte variable. */

    handle->lastRxByte = 0;

    /* Reset receive byte ring buffer index to zero. */

    handle->rxByteRingBufferIndex = 0;

    /* Zero initialize RX byte ring buffer. */

    memset(handle->rxByteRingBuffer, 0 , sizeof(handle->rxByteRingBuffer));
}

/**
 *  @brief              Add an encoded SSCP packet into request FIFO.
 *  @param[in]  handle  Pointer to SSCP instance.
 *  @param[in]  packet  Pointer to packet to be stored in the request FIFO.
 *  @return             None.
 *  @details            This is a private function used to add a received SSCP packet
 *                      into the FIFO. This function is called by SSCP_handleRxByte.

 */

static void SSCP_requestFifoPut(SSCP_Handle* handle, SSCP_encodedPacket* packet)
{
    if( handle->requestFifoStatus == SSCP_REQUEST_FIFO_FULL )
    {
        return;
    }

    /* FIFO status is OK. */

    handle->requestFifoStatus = SSCP_REQUEST_FIFO_OK;

    /* Copy contents of packet into FIFO. */

    memcpy( &handle->requestFifo[handle->requestFifoPutIndex], \
            packet, sizeof(SSCP_encodedPacket) );

    /* Increment put index. */

    handle->requestFifoPutIndex++;

    /* Wrap around put index. */

    if( handle->requestFifoPutIndex == SSCP_REQUEST_FIFO_SIZE )
    {
        handle->requestFifoPutIndex = 0;
    }

    /* Increment element count of FIFO. */

    handle->requestFifoCount++;

    /* If element count equals FIFO size, change queue status to full. */

    if( handle->requestFifoCount == SSCP_REQUEST_FIFO_SIZE )
    {
        handle->requestFifoStatus = SSCP_REQUEST_FIFO_FULL;
    }
}

/**
 *  @brief              Get a SSCP packet from the request FIFO.
 *  @param[in]  handle  Pointer to SSCP instance.
 *  @param[out]  packet Pointer to packet to be stored in the request FIFO.
 *  @return             None.
 *  @details            This is a private function used to get a SSCP packet
 *                      from the FIFO. This function is called by SSCP_process.

 */

static void SSCP_requestFifoGet(SSCP_Handle* handle, SSCP_encodedPacket* packet)
{
    if( handle->requestFifoStatus == SSCP_REQUEST_FIFO_EMPTY )
    {
        return;
    }

    /* FIFO status is OK. */

    handle->requestFifoStatus = SSCP_REQUEST_FIFO_OK;

    /* Copy packet from request FIFO into destination packet location. */

    memcpy( packet, &handle->requestFifo[handle->requestFifoGetIndex], sizeof(SSCP_encodedPacket) );

    /* Increment get index. */

    handle->requestFifoGetIndex++;

    /* Decrement element count as a packet has been removed from FIFO. */

    handle->requestFifoCount--;

    /* Wrap around get index. */

    if( handle->requestFifoGetIndex == SSCP_REQUEST_FIFO_SIZE )
    {
        handle->requestFifoGetIndex = 0;
    }

    /* If no elements present in FIFO, set status to empty. */

    if( handle->requestFifoCount == 0 )
    {
        handle->requestFifoStatus = SSCP_REQUEST_FIFO_EMPTY;
    }
}


/**
 * @brief               Calculates CRC8-CCITT of \p data of size \p size.
 * @param[in]   data    A pointer to a uint8_t array containing data whose CRC has to be calculated.
 * @param[in]   size    Number of bytes occupied by the variable at location pointed by \p data.
 * @return      None
 */

static const uint8_t crc8_ccitt_table[256] = {
    0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,
    0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D,
    0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,
    0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,
    0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,
    0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,
    0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,
    0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,
    0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,
    0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,
    0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,
    0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,
    0x27,0x20,0x29,0x2E,0x3B,0x3C,0x35,0x32,
    0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A,
    0x57,0x50,0x59,0x5E,0x4B,0x4C,0x45,0x42,
    0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A,
    0x89,0x8E,0x87,0x80,0x95,0x92,0x9B,0x9C,
    0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4,
    0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,0xEC,
    0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4,
    0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,
    0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44,
    0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,
    0x21,0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34,
    0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,
    0x76,0x71,0x78,0x7F,0x6A,0x6D,0x64,0x63,
    0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,
    0x06,0x01,0x08,0x0F,0x1A,0x1D,0x14,0x13,
    0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,
    0x96,0x91,0x98,0x9F,0x8A,0x8D,0x84,0x83,
    0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,
    0xE6,0xE1,0xE8,0xEF,0xFA,0xFD,0xF4,0xF3
};

static uint8_t SSCP_CRC8CCITT(uint8_t* data, size_t size)
{
    uint8_t crc = 0;

    while (size--)
    {
        crc = crc8_ccitt_table[crc ^ *data++];
    }

    return crc;
}

/**
 *  @brief                      Encodes the packet information given in \p packetInfo and stores
 *                              encoded packet in \p packet.
 *  @param[in]  packetInfo      Pointer to packet information structure.
 *  @param[out] packet          Pointer to structure which holds encoded packet.
 *  @return                     None
 */

void SSCP_encodePacket(SSCP_packetInfo* packetInfo, SSCP_encodedPacket* packet)
{
    static uint8_t unencodedPacket[9] = {0};

    /* Add start of packet and end of packet bytes. */

    unencodedPacket[0] = SSCP_START_OF_PACKET_BYTE;
    unencodedPacket[8] = SSCP_END_OF_PACKET_BYTE;

    /* Form byte 1 as per protocol specifications. */

    unencodedPacket[1] = ( packetInfo->deviceID << 3 ) |    \
                         ( packetInfo->ack << 2 )      |    \
                         ( packetInfo->read << 1 )     |    \
                         ( packetInfo->write );
    
    /* Set register ID. */

    unencodedPacket[2] = packetInfo->regID;

    /* Set register Data. */

    unencodedPacket[3]  =   (uint8_t)( ( packetInfo->regData & ((uint32_t)0xFF000000U) ) >> 24 );
    unencodedPacket[4]  =   (uint8_t)( ( packetInfo->regData & ((uint32_t)0x00FF0000U) ) >> 16 );
    unencodedPacket[5]  =   (uint8_t)( ( packetInfo->regData & ((uint32_t)0x0000FF00U) ) >> 8 );
    unencodedPacket[6]  =   (uint8_t)( ( packetInfo->regData & ((uint32_t)0x000000FFU) ) );

    /* Add CRC-8 CCITT. */

    unencodedPacket[7]  =   SSCP_CRC8CCITT( &unencodedPacket[1], 6 );
    
    /* Convert the unencoded packet into ASCII encoded format. */

    memset( packet, 0, sizeof(*packet) );

    /* Add start of packet and end of packet bytes. */

    packet->byte[0]  = unencodedPacket[0];
    packet->byte[15] = unencodedPacket[8];
    
    /* ASCII encode other bytes. */

    packet->byte[1] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[1] & 0xF0) >> 4 );
    packet->byte[2] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[1] & 0x0F) );
    packet->byte[3] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[2] & 0xF0) >> 4 );
    packet->byte[4] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[2] & 0x0F) );
    packet->byte[5] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[3] & 0xF0) >> 4 );
    packet->byte[6] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[3] & 0x0F) );
    packet->byte[7] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[4] & 0xF0) >> 4 );
    packet->byte[8] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[4] & 0x0F) );
    packet->byte[9] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[5] & 0xF0) >> 4 );
    packet->byte[10] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[5] & 0x0F) );
    packet->byte[11] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[6] & 0xF0) >> 4 );
    packet->byte[12] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[6] & 0x0F) );
    packet->byte[13] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[7] & 0xF0) >> 4 );
    packet->byte[14] = SSCP_NIBBLE_TO_HEXCHAR( (unencodedPacket[7] & 0x0F) );
}

/**
 *  @brief                      Decodes the packet given in \p packet and stores
 *                              packet information in \p packetInfo.
 *  @param[out]  packetInfo     Pointer to packet information structure.
 *  @param[in] packet           Pointer to structure which holds encoded packet.
 *  @return                     1 if decoding is successful, 0 if failed.
 */

int SSCP_decodePacket(SSCP_packetInfo* packetInfo, SSCP_encodedPacket* packet)
{
    /* Convert encoded bytes to unencoded bytes. */

    static uint8_t unencodedPacket[9] = {0};

    unencodedPacket[0] = packet->byte[0];
    unencodedPacket[8] = packet->byte[15];

    /* If start of packet byte and end of packet byte do not comply, return 0. */

    if( unencodedPacket[0] != SSCP_START_OF_PACKET_BYTE )
    {
        return 0;
    }

    if( unencodedPacket[8] != SSCP_END_OF_PACKET_BYTE )
    {
        return 0;
    }

    /* Extract data from unencoded bytes. */

    uint8_t lowNibble;
    uint8_t highNibble;

    unencodedPacket[1]  = ( (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[1]) << 4 );
    unencodedPacket[1] |= (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[2]);

    unencodedPacket[2]  = ( (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[3]) << 4 );
    unencodedPacket[2] |= (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[4]);

    unencodedPacket[3]  = ( (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[5]) << 4 );
    unencodedPacket[3] |= (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[6]);

    unencodedPacket[4]  = ( (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[7]) << 4 );
    unencodedPacket[4] |= (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[8]);

    unencodedPacket[5]  = ( (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[9]) << 4 );
    unencodedPacket[5] |= (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[10]);

    unencodedPacket[6]  = ( (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[11]) << 4 );
    unencodedPacket[6] |= (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[12]);

    unencodedPacket[7]  = ( (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[13]) << 4 );
    unencodedPacket[7] |= (uint8_t)SSCP_HEXCHAR_TO_NIBBLE(packet->byte[14]);

    /* Populate the packet information structure. */

    packetInfo->deviceID = ((unencodedPacket[1] >> 3) & 0x1F);
    packetInfo->ack      = ((unencodedPacket[1] >> 2) & 0x01);
    packetInfo->read     = ((unencodedPacket[1] >> 1) & 0x01);
    packetInfo->write    = (unencodedPacket[1] & 0x01);

    packetInfo->regID    = unencodedPacket[2];

    packetInfo->regData = 0;

    packetInfo->regData |= ( ((uint32_t)unencodedPacket[3]) << 24 );
    packetInfo->regData |= ( ((uint32_t)unencodedPacket[4]) << 16 );
    packetInfo->regData |= ( ((uint32_t)unencodedPacket[5]) << 8 );
    packetInfo->regData |= ( ((uint32_t)unencodedPacket[6]) );

    packetInfo->crc8 = unencodedPacket[7];

    uint8_t crc8;

    /* Calculate CRC8 for unencoded bytes 1 to 7 (0-indexed). */

    crc8 = SSCP_CRC8CCITT( &unencodedPacket[1], 7 );

    /* CRC8 must be zero. */

    if(crc8 == 0)
    {
        return 1;
    }

    return 0;
}

void SSCP_handleRxByte(SSCP_Handle* handle, uint8_t byte)
{
    static SSCP_packetInfo SSCP_requestPacketInfo = {0};
    int ret;

    memset( &SSCP_requestPacketInfo, 0, sizeof(SSCP_packetInfo) );

    handle->lastRxByte = byte;

    if( handle->rxByteRingBufferIndex < 16 )
    {
        handle->rxByteRingBuffer[handle->rxByteRingBufferIndex] = byte;

        handle->rxByteRingBufferIndex++;

        /* If termination character is received and if sufficient bytes are not received, do a reset. */

        if( ( byte == SSCP_END_OF_PACKET_BYTE ) && ( handle->rxByteRingBufferIndex != 16 ) )
        {
            handle->rxByteRingBufferIndex = 0;
            memset( handle->rxByteRingBuffer, 0, sizeof(handle->rxByteRingBuffer) );
        }

        /* Check if a complete packet is received. */

        if( handle->rxByteRingBufferIndex == 16 )
        {
            /* Check if start of packet and end of packet characters are correct. */

            if( ( handle->rxByteRingBuffer[0] == SSCP_START_OF_PACKET_BYTE ) &&    \
                ( handle->rxByteRingBuffer[15] == SSCP_END_OF_PACKET_BYTE ) )
                {
                    /* Before storing in FIFO, check if received packet belongs to
                       the current device. */

                    ret = SSCP_decodePacket( &SSCP_requestPacketInfo, (SSCP_encodedPacket*)handle->rxByteRingBuffer );

                    if( ret && ( handle->deviceID == SSCP_requestPacketInfo.deviceID ) )
                    {
                        /* Push received packet into FIFO. */

                        SSCP_requestFifoPut(handle, (SSCP_encodedPacket*)handle->rxByteRingBuffer);
                    }
                }
                

            /* Reset RX byte ring buffer. */

            handle->rxByteRingBufferIndex = 0;

            memset( handle->rxByteRingBuffer, 0, sizeof(handle->rxByteRingBuffer) );
        }
    }
}


int SSCP_process(SSCP_Handle* handle)
{
    int ret = 0;
    uint32_t readRegisterValue = 0;
    int index = 0;

    static SSCP_encodedPacket packet = {0};

    static SSCP_packetInfo encodedPacketInfo = {0};

    static SSCP_packetInfo decodedPacketInfo = {0};

    memset( &packet, 0, sizeof(packet) );

    /* If the FIFO is empty, do nothing. */

    if( handle->requestFifoStatus == SSCP_REQUEST_FIFO_EMPTY )
    {
        return 0;
    }

    /* Get a packet from the FIFO. */

    SSCP_requestFifoGet(handle, &packet);

    /* Decode packet got from FIFO. */

    ret = SSCP_decodePacket( &decodedPacketInfo, &packet );

    /* If decoding fails, return false. */

    if( ret == 0 )
    {
        return ret;
    }

    /* Go through the register handles list and execute callback function if any. */

    for( index = 0 ; handle->registerHandlesCount ; index++ )
    {
        /* If decoded register ID matches with register handle register ID. */

        if( handle->registerHandles[index].regID == decodedPacketInfo.regID )
        {
            /* If operation is a write operation, then, write corresponding register value. */

            if( decodedPacketInfo.write )
            {
                *((uint32_t*)handle->registerHandles[index].pData) = decodedPacketInfo.regData;
            }

            /* Check if callback is provided. If provided, execute it. */

            if( handle->registerHandles[index].callback )
            {
                /* Read operation. */

                if( decodedPacketInfo.read && !decodedPacketInfo.write )
                {
                    handle->registerHandles[index].callback( handle->registerHandles[index].pData, SSCP_REGISTER_OPERATION_READ );
                }
                else if( !decodedPacketInfo.read && decodedPacketInfo.write )   /* Write operation. */
                {
                    handle->registerHandles[index].callback( handle->registerHandles[index].pData, SSCP_REGISTER_OPERATION_WRITE );
                }

                readRegisterValue = *((uint32_t*)handle->registerHandles[index].pData);
            }

            break;
        }
    }

    /* Form the response packet. */

    memcpy( &encodedPacketInfo, &decodedPacketInfo, sizeof(SSCP_packetInfo) );

    /* Set ACK bit. */

    encodedPacketInfo.ack = 1;

    /* If it was a read operation, then, get the updated register value. */

    if( decodedPacketInfo.read )
    {
        encodedPacketInfo.regData = readRegisterValue;
    }

    memset( &packet, 0, sizeof(packet) );

    /* Encode the packet. */

    SSCP_encodePacket( &encodedPacketInfo, &packet );

    /* Call link specific send function to send over the response packet. */

    if( handle->send )
        return handle->send( (uint8_t*)&packet.byte[0], sizeof(packet) );

    return 0;
}


#define SSCP_REQUEST_FIFO_STATUS_STRING(x)                                          \
    ( x == SSCP_REQUEST_FIFO_OK ) ? "OK" : ( x == SSCP_REQUEST_FIFO_EMPTY ) ?       \
    "EMPTY" : "FULL"                                                                \

void SSCP_printInfo(SSCP_Handle* handle)
{
    int index;

    printf("\n\nSSCP handle information:\n\n");

    printf("***** Rx Byte Ring Buffer details *****\n\n");

    printf("Last received byte: %x\n", handle->lastRxByte);

    printf("Buffer index: %d\n", handle->rxByteRingBufferIndex);

    printf("Contents of ring buffer:\n");

    for( index = 0 ; index < 16 ; index++ )
    {
        printf("%.2x ", handle->rxByteRingBuffer[index]);
    }

    printf("\n");

    printf("***** Request FIFO details *****\n");

    printf("Get index: %d\n", handle->requestFifoGetIndex);
    printf("Put index: %d\n", handle->requestFifoPutIndex);

    printf("FIFO status: %s\n", SSCP_REQUEST_FIFO_STATUS_STRING(handle->requestFifoStatus));

    printf("Element count: %d\n", handle->requestFifoCount);

    printf("Contents of Request FIFO:\n\n");

    for( index = 0 ; index < sizeof(handle->requestFifo)/sizeof(handle->requestFifo[0]) ; index++ )
    {
        printf("Packet [%d]: ", index);
        printf("%.2x ", handle->requestFifo[index].byte[0]);
        printf("%.2x ", handle->requestFifo[index].byte[1]);
        printf("%.2x ", handle->requestFifo[index].byte[2]);
        printf("%.2x ", handle->requestFifo[index].byte[3]);
        printf("%.2x ", handle->requestFifo[index].byte[4]);
        printf("%.2x ", handle->requestFifo[index].byte[5]);
        printf("%.2x ", handle->requestFifo[index].byte[6]);
        printf("%.2x ", handle->requestFifo[index].byte[7]);
        printf("%.2x ", handle->requestFifo[index].byte[8]);
        printf("%.2x ", handle->requestFifo[index].byte[9]);
        printf("%.2x ", handle->requestFifo[index].byte[10]);
        printf("%.2x ", handle->requestFifo[index].byte[11]);
        printf("%.2x ", handle->requestFifo[index].byte[12]);
        printf("%.2x ", handle->requestFifo[index].byte[13]);
        printf("%.2x ", handle->requestFifo[index].byte[14]);
        printf("%.2x ", handle->requestFifo[index].byte[15]);
        
        if( index == handle->requestFifoGetIndex )
        {
            printf(" (GET)");
        }
        
        if( index == handle->requestFifoPutIndex )
        {
            printf(" (PUT)");
        }
        

        printf("\n");
    }

    printf("\n\n");
}