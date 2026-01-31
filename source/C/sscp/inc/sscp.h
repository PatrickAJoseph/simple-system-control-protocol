
#ifndef SSCP_H
#define SSCP_H

#include <stdio.h>
#include <stdint.h>

/* Number of packets which can be held by the request FIFO. */

#ifndef SSCP_REQUEST_FIFO_SIZE
#define SSCP_REQUEST_FIFO_SIZE      8
#endif

/* Status words for request FIFO. */

#define SSCP_REQUEST_FIFO_OK        0
#define SSCP_REQUEST_FIFO_EMPTY     1
#define SSCP_REQUEST_FIFO_FULL      2

/* Typedef for encoded packet. */

typedef struct sscp_encoded_packet_struct 
{

    uint8_t byte[16];

}SSCP_encodedPacket;


/**
 *  @brief              Handle for an SSCP instance.
 *  
 * 
 */

typedef struct sscp_handle
{
    /* FIFO to hold encoded packets received through interface. */

    SSCP_encodedPacket requestFifo[SSCP_REQUEST_FIFO_SIZE];

    /* Put index for the request FIFO. This is index in the FIFO to which the
       producer function writes a packet to. */

    int requestFifoPutIndex;

    /* Get index for the request FIFO. This is the index in the FIFO from which the
       consumer function reads a packet from. */

    int requestFifoGetIndex;

    /* Number of packets in the request FIFO. */

    int requestFifoCount;

    /*  Request FIFO status.
    *   0: Request FIFO OK.
    *   1: Request FIFO empty.
    *   2: Request FIFO full.
    */

    int requestFifoStatus;

    /* Last received byte through link. */

    uint8_t lastRxByte;

    /* Receive byte ring buffer index. */

    int rxByteRingBufferIndex;

    /* Ring buffer for receiving bytes. */

    uint8_t rxByteRingBuffer[16];

}SSCP_Handle;

/**
*  @brief               Initialize SSCP stack.
*  @param[in]   handle  Pointer to SSCP instance.
*  @return              None
*
*  @details             Cleans the SSCP request FIFO and byte receive ring buffer.
*
*/

void SSCP_init(SSCP_Handle* handle);

/**
 * @brief               Processes the received byte from an interface.
 * @param[in]   handle  Pointer to SSCP instance.
 * @param[in]   byte    Received byte.
 * @return              None
 * 
 * @details             Call this function within the function which received bytes from the link interface.
 *                      This function loads the received byte into a ring buffer, checks if frame is valid and
 *                      if valid, pushes the packet into the request FIFO.
 *  */

void SSCP_handleRxByte(SSCP_Handle* handle, uint8_t byte);

/**
 *  @brief              Dequeues a packet from the FIFO and processes the packet.
 *  @param[in]  handle  Pointer to SSCP instance.
 *  @return             Returns 1 if processed a packet, else returns 0 indicating no packets available to process.
 *  @details            This function must be called only from thread context. Do NOT call it from
 *                      process context.
 */

 int SSCP_process(SSCP_Handle* handle);

#ifdef SSCP_DEBUG

/**
 *  @brief              Function to print details about the SSCP instance.
 *  @param[in]  handle  Pointer to SSCP instance.
 *  @details            Print information about receive ring buffer and contents of
 *                      request packet FIFO. Should be enabled by adding the compiler
 *                      flag -DSSCP_DEBUG
 */

void SSCP_printInfo(SSCP_Handle* handle);

#endif /* SSCP_DEBUG */

 #endif /* SSCP_H */