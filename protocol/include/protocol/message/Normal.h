#pragma once

#include "../base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint32_t uid;
    uint8_t messageType;
    //USER_MESSAGE_TYPE
    time_t time;
    uint16_t messageLen;
    char message[0];
} CRPPacketMessageNormal;

__attribute_malloc__
CRPPacketMessageNormal *CRPMessageNormalCast(CRPBaseHeader *base);

int CRPMessageNormalSend(CRPContext context,
                         uint32_t sessionID,
                         USER_MESSAGE_TYPE messageType,
                         uint32_t uid,
                         time_t time,
                         uint16_t messageLen,
                         char *message);
