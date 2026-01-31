
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../inc/sscp.h"

#define SSCP_START_OF_PACKET_BYTE     ((uint8_t)'*')
#define SSCP_END_OF_PACKET_BYTE       ((uint8_t)'#')


void SSCP_init(SSCP_Handle* handle)
{
    /* Check SSCP handle. */

    if(handle == NULL)
    {
        return NULL;
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

static void SSCP_requestFifoPut(SSCP_Handle* handle, SSCP_encodedPacket* packet)
{
    if( handle->requestFifoStatus == SSCP_REQUEST_FIFO_FULL )
    {
        return;
    }

    handle->requestFifoStatus = SSCP_REQUEST_FIFO_OK;

    memcpy( &handle->requestFifo[handle->requestFifoPutIndex], \
            packet, sizeof(SSCP_encodedPacket) );

    handle->requestFifoPutIndex++;

    if( handle->requestFifoPutIndex == SSCP_REQUEST_FIFO_SIZE )
    {
        handle->requestFifoPutIndex = 0;
    }

    handle->requestFifoCount++;

    if( handle->requestFifoCount == SSCP_REQUEST_FIFO_SIZE )
    {
        handle->requestFifoStatus = SSCP_REQUEST_FIFO_FULL;
    }
}

static void SSCP_requestFifoGet(SSCP_Handle* handle, SSCP_encodedPacket* packet)
{
    if( handle->requestFifoStatus == SSCP_REQUEST_FIFO_EMPTY )
    {
        return;
    }

    handle->requestFifoStatus = SSCP_REQUEST_FIFO_OK;

    memcpy( packet, &handle->requestFifo[handle->requestFifoGetIndex], sizeof(SSCP_encodedPacket) );

    handle->requestFifoGetIndex++;

    handle->requestFifoCount--;

    if( handle->requestFifoGetIndex == SSCP_REQUEST_FIFO_SIZE )
    {
        handle->requestFifoGetIndex = 0;
    }

    if( handle->requestFifoCount == 0 )
    {
        handle->requestFifoStatus = SSCP_REQUEST_FIFO_EMPTY;
    }
}

void SSCP_handleRxByte(SSCP_Handle* handle, uint8_t byte)
{
    handle->lastRxByte = byte;

    if( handle->rxByteRingBufferIndex < 16 )
    {
        handle->rxByteRingBuffer[handle->rxByteRingBufferIndex] = byte;

        handle->rxByteRingBufferIndex++;

        /* Check if a complete packet is received. */

        if( handle->rxByteRingBufferIndex == 16 )
        {
            /* Check if start of packet and end of packet characters are correct. */

            if( ( handle->rxByteRingBuffer[0] == SSCP_START_OF_PACKET_BYTE ) &&    \
                ( handle->rxByteRingBuffer[15] == SSCP_END_OF_PACKET_BYTE ) )
                {
                    /* Push received packet into FIFO. */

                    SSCP_requestFifoPut(handle, (SSCP_encodedPacket*)handle->rxByteRingBuffer);
                }
                

            /* Reset RX byte ring buffer. */

            handle->rxByteRingBufferIndex = 0;

            memset( handle->rxByteRingBuffer, 0, sizeof(handle->rxByteRingBuffer) );
        }
    }
}

int SSCP_process(SSCP_Handle* handle)
{
    
}


#define SSCP_REQUEST_FIFO_STATUS_STRING(x)                                          \
    ( x == SSCP_REQUEST_FIFO_OK ) ? "OK" : ( x == SSCP_REQUEST_FIFO_EMPTY ) ?       \
    "EMPTY" : "OK"                                                                  \

#ifdef SSCP_DEBUG

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
        printf("%x ", handle->rxByteRingBuffer[index]);
    }

    printf("\n");

    printf("***** Request FIFO details *****\n");

    printf("Get index: %d\n", handle->requestFifoGetIndex);
    printf("Put index: %d\n", handle->requestFifoPutIndex);

    printf("FIFO status: %s\n", SSCP_REQUEST_FIFO_STATUS_STRING(handle->requestFifoStatus));

    printf("Element count: %d\n", handle->requestFifoCount);

    printf("Contents of Request FIFO:\n\n");

    for( index = 0 ; index < sizeof(handle->requestFifo)/szieof(handle->requestFifo[0]) ; index++ )
    {
        printf("Packet [%d]: ", index);
        printf("%x ", handle->requestFifo[index].byte[15]);
        printf("%x ", handle->requestFifo[index].byte[14]);
        printf("%x ", handle->requestFifo[index].byte[13]);
        printf("%x ", handle->requestFifo[index].byte[12]);
        printf("%x ", handle->requestFifo[index].byte[11]);
        printf("%x ", handle->requestFifo[index].byte[10]);
        printf("%x ", handle->requestFifo[index].byte[9]);
        printf("%x ", handle->requestFifo[index].byte[8]);
        printf("%x ", handle->requestFifo[index].byte[7]);
        printf("%x ", handle->requestFifo[index].byte[6]);
        printf("%x ", handle->requestFifo[index].byte[5]);
        printf("%x ", handle->requestFifo[index].byte[4]);
        printf("%x ", handle->requestFifo[index].byte[3]);
        printf("%x ", handle->requestFifo[index].byte[2]);
        printf("%x ", handle->requestFifo[index].byte[1]);
        printf("%x ", handle->requestFifo[index].byte[0]);
        
        if( index == handle->requestFifoGetIndex )
        {
            printf(" (GET)\n");
        }
        else if( index == handle->requestFifoPutIndex )
        {
            printf(" (PUT)\n");
        }
        else
        {
            printf("\n");
        }
    }

    printf("\n\n");
}

#endif /* SSCP_DEBUG */