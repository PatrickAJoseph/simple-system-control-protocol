
#include <stdio.h>
#include <stdlib.h>

#include "../inc/sscp.h"

/* TEST PARAMETERS */

#define SSCP_TEST_RX_HANDLER_PUT_COUNT          5
#define SSCP_TEST_RX_HANDLER_GET_COUNT          3

SSCP_Handle handle;

const char* testPackets[] = {
    "*00000000000000#",
    "*00000000000001#",
    "*00000000000002#",
    "*00000000000003#",
    "*00000000000004#",
    "*00000000000005#",
    "*00000000000006#",
    "*00000000000007#",
};

SSCP_encodedPacket getPacket;

int main(void)
{
    SSCP_init(&handle);

    for(int packetIndex = 0 ; packetIndex < SSCP_TEST_RX_HANDLER_PUT_COUNT; packetIndex++ )
    {
        printf("\nPutting test packet %d\n", packetIndex);

        for( int byteIndex = 0 ; byteIndex < 16 ; byteIndex++ )
        {
            printf("%.2x ", testPackets[packetIndex][byteIndex]);
            SSCP_handleRxByte( &handle, testPackets[packetIndex][byteIndex] );
        }

        printf("\n");
    }

    SSCP_printInfo(&handle);

    return 0;
}