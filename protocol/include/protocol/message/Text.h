#pragma once

#include "../base.h"
#include <stdint.h>

CRP_STRUCTURE
{
    uint32_t touid;
    uint16_t message_len;
    char message[0];
} CRPPacketMessageText;

__attribute_malloc__
CRPPacketMessageText *CRPMessageTextCast(CRPBaseHeader *base);

int CRPMessageTextSend(int sockfd, uint32_t sessionID, uint32_t userid, uint16_t message_len, char *message);
