#pragma once

#include "../base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint32_t uid;
    uint8_t messageType;
    //USER_MESSAGE_TYPE
    uint16_t messageLen;
    char message[0];
} CRPPacketMessageText;

__attribute_malloc__
CRPPacketMessageText *CRPMessageTextCast(CRPBaseHeader *base);

int CRPMessageTextSend(int sockfd, uint32_t sessionID, USER_MESSAGE_TYPE messageType, uint32_t uid, uint16_t messageLen, char *message);
