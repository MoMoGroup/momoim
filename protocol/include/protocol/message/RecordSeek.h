#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint32_t date;
} CRPPacketMessageRecordNext;

__attribute_malloc__
CRPPacketMessageRecordNext *CRPMessageRecordNextCast(CRPBaseHeader *base);

int CRPMessageRecordNextSend(CRPContext context, uint32_t sessionID, uint32_t uid, uint8_t size);
