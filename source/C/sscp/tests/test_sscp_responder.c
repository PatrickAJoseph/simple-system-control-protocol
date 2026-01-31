
#include <stdio.h>
#include <stdint.h>

#include "../inc/sscp.h"


int send(uint8_t* data, size_t size)
{
    printf("\n\nSending following packet:\n");

    int index;

    for( index = 0 ; index < size ; index++ )
    {
        printf("%.2x ", data[index]);
    }

    printf("\n");
}

uint32_t register0Data;

void register0_callback(void* reg, int operation)
{
    printf("Register 0 handle called !\n");
}

SSCP_REGISTER_HANDLE_LIST(sscpTestHandleList)
{
    SSCP_REGISTER_HANDLE( 0, register0Data, register0_callback ),
};

static SSCP_HANDLE(sscpTestHandle,SSCP_REGISTER_HANDLE_LIST_NAME(sscpTestHandleList),send, 0);


/* List of test request packets. */

const char* testRequestPackets[] = {
    "*02000000192351#",
};

int main(void)
{
    for( int requestPacketIndex = 0 ; requestPacketIndex < sizeof(testRequestPackets)/sizeof(testRequestPackets[0]) ; requestPacketIndex++ )
    {
        for( int requestPacketByteIndex ; requestPacketByteIndex < 16 ; requestPacketByteIndex++ )
            SSCP_handleRxByte(&sscpTestHandle, testRequestPackets[requestPacketIndex][requestPacketByteIndex]);
    }

    if( SSCP_process(&sscpTestHandle) )
    {
        printf("SSCP: Process executed properly");
    }
    else
    {
        printf("SSCP: Error in running SSCP responder stack.");
    }

    return 0;
}