#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint8_t remain;
    UserMessage messages;
} CRPPacketMessageRecordData;

__attribute_malloc__
CRPPacketMessageRecordData *CRPMessageRecordDataCast(CRPBaseHeader *base);

int CRPMessageRecordDataSend(CRPContext context, uint32_t sessionID, uint8_t remain, UserMessage *message);
