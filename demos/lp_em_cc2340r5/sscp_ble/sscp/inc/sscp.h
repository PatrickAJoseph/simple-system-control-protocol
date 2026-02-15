
#ifndef SSCP_H
#define SSCP_H

#include <stdio.h>
#include <stdint.h>

#define SSCP_REGISTER_HANDLE_LIST_NAME(name)                                        \
    SSCP_registerHandle_##name

/* Define a SSCP register handle list. */

#define SSCP_REGISTER_HANDLE_LIST(name)                                             \
    SSCP_registerHandle SSCP_registerHandle_##name[] =

/* Define a SSCP register handle. */

#define SSCP_REGISTER_HANDLE(registerID, registerData, registerCallback)            \
    {                                                                               \
        .regID  = registerID,                                                       \
        .pData  = (void*)&registerData,                                             \
        .callback = registerCallback,                                               \
    }

/* Defines an SSCP handle. */

#define SSCP_HANDLE(name, registerHandleList, sendHandler, deviceID)                                            \
                                                                                                                \
SSCP_Handle  name = {                                                                                           \
    .requestFifo                        =   {0},                                                                \
    .requestFifoGetIndex                =   0,                                                                  \
    .requestFifoPutIndex                =   0,                                                                  \
    .requestFifoCount                   =   0,                                                                  \
    .requestFifoStatus                  =   SSCP_REQUEST_FIFO_EMPTY,                                            \
    .lastRxByte                         =   0,                                                                  \
    .rxByteRingBuffer                   =   {0},                                                                \
    .rxByteRingBufferIndex              =   0,                                                                  \
    .registerHandles                    =   registerHandleList,                                                 \
    .registerHandlesCount               =   sizeof(registerHandleList)/sizeof(registerHandleList[0]),           \
    .send                               =   sendHandler,                                                        \
}

/* Number of packets which can be held by the request FIFO. */

#ifndef SSCP_REQUEST_FIFO_SIZE
#define SSCP_REQUEST_FIFO_SIZE      8
#endif

/* Status words for request FIFO. */

#define SSCP_REQUEST_FIFO_OK        0
#define SSCP_REQUEST_FIFO_EMPTY     1
#define SSCP_REQUEST_FIFO_FULL      2

#define SSCP_REGISTER_OPERATION_READ        0
#define SSCP_REGISTER_OPERATION_WRITE       1

/**
 *  @brief                  Function typedef for SSCP register callback.  
 *  @param[in]  reg         Pointer to variable holding register value.
 *  @param[in]  operation   Operation being done on the register.
 *                          \p operation can take value SSCP_REGISTER_OPERATION_READ or
 *                          SSCP_REGISTER_OPERTION_WRITE.
 *  @return                 None.  
 */

typedef void (*SSCP_registerCallback)(void* pReg, int operation);

/* SSCP register handler structure. */

typedef struct sscp_register_handler_struct
{
    /* Register ID. */

    const int regID;

    /* Pointer to vaiable holding register value. */

    void* pData;

    /* Callback function. */

    SSCP_registerCallback callback;

}SSCP_registerHandle;

/* Typedef for encoded packet. */

typedef struct sscp_encoded_packet_struct 
{

    uint8_t byte[16];

}SSCP_encodedPacket;

/**
 *  Structure for holding information about a packet.
 *  
 *  This structure is used to store information about a packet 
 *  after decoding of the packet using SSCP_decodePacket and also 
 *  passed as a parameter to the packet encoding function using SSCP_encodePacket.
 */

typedef struct SSCP_packetInfo
{
    /* Device ID of the initiator. This is used to filter out packets based on device ID. */

    uint8_t deviceID;

    /* Acknowledgement bit which is set during response. */

    uint8_t ack;

    /* Read bit: Set if initiator requests a read operation. */

    uint8_t read;

    /* Write bit: Set if initiator requests a write operation. */

    uint8_t write;

    /* Register ID: The register number which the initiator wants to access. */

    uint8_t regID;

    /* Register data */

    uint32_t regData;

    /* CRC8-CCITT of [(deviceID << 3)|(ack<<2)|(read<<1)|(write<<0), regID, regData]*/

    uint8_t crc8;

}SSCP_packetInfo;


/**
 *  @brief              Handle for an SSCP instance.
 *  
 * 
 */

typedef struct sscp_handle
{
    /* Device ID */

    const int deviceID;

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

    /* Pointer to list of register handle structure. */

    SSCP_registerHandle* registerHandles;

    /* Number of entries in register handle list. */

    int registerHandlesCount;

    /* Function to send response packet. */

    /* This callback function must be provided by the user based on the 
       interface they are using (serial, BT etc.) */

    int (*send)(uint8_t* data, size_t size);

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

/**
 *  @brief              Function to print details about the SSCP instance.
 *  @param[in]  handle  Pointer to SSCP instance.
 *  @details            Print information about receive ring buffer and contents of
 *                      request packet FIFO. Should be enabled by adding the compiler
 *                      flag -DSSCP_DEBUG
 */

void SSCP_printInfo(SSCP_Handle* handle);

 #endif /* SSCP_H */