#pragma once

#include <sys/types.h>

typedef __uint16_t CRP_LENGTH_TYPE;

typedef struct
{
    unsigned int magicCode /* 0x464F5573 */;
    CRP_LENGTH_TYPE totalLength;
    CRP_LENGTH_TYPE dataLength;
    int packetID;
    char data[0];
} CRPBaseHeader;

#define CRP_PACKET_MAX 4096
#define CRP_PACKET_CAST(header, toType) ((toType*)header->data)

//pack protocol data
ssize_t CRPSend(int packetID, void *data, size_t length, int fd);

ssize_t CRPRecv(CRPBaseHeader **header, int fd);