
#include <stdio.h>
#include <stdlib.h>

#include "../inc/sscp.h"

extern void SSCP_encodePacket(SSCP_packetInfo* packetInfo, SSCP_encodedPacket* packet);
extern void SSCP_decodePacket(SSCP_packetInfo* packetInfo, SSCP_encodedPacket* packet);

static void printEncodedPacket(SSCP_encodedPacket* packet)
{
    printf("\n\nEncoded packet:");

    int index;

    for( index = 0 ; index < sizeof(*packet) ; index++ )
    {
        printf("%.2x ", packet->byte[index]);
    }

    printf("\n");
}

static void printPacketInfo(SSCP_packetInfo* packetInfo)
{
    printf("\n\nPacket information\n\n");

    printf("Device ID: %d\n", packetInfo->deviceID);
    printf("ACK: %d\n", packetInfo->ack);
    printf("READ: %d\n", packetInfo->read);
    printf("WRITE: %d\n", packetInfo->write);
    printf("Register ID: %d\n", packetInfo->regID);
    printf("Register data: 0x%.8x\n", packetInfo->regData);
    printf("CRC8: %.2x\n\n", packetInfo->crc8);
}

static SSCP_packetInfo encodedPacketInfo = {0};
static SSCP_packetInfo decodedPacketInfo = {0};
static SSCP_encodedPacket encodedPacket = {0};

int main(void)
{
    encodedPacketInfo.deviceID = 10;
    encodedPacketInfo.ack = 0;
    encodedPacketInfo.read = 1;
    encodedPacketInfo.write = 0;
    encodedPacketInfo.regID = 2;
    encodedPacketInfo.regData = 0x3284;
    
    SSCP_encodePacket(&encodedPacketInfo, &encodedPacket);

    printEncodedPacket(&encodedPacket);

    SSCP_decodePacket(&decodedPacketInfo, &encodedPacket);

    printPacketInfo(&decodedPacketInfo);

    return 0;
}