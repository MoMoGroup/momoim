#pragma once

#include "stdint.h"
typedef struct
{
    uint8_t protocolVersion;
    uint32_t clientVersion;
    uint32_t sessionState;
} CRPPacketHello;


CRPPacketHello *CRPHelloCast(CRPBaseHeader *base);

int CRPHelloSend(int sockfd, uint8_t protocolVersion, uint32_t clientVersion, uint32_t sessionState);