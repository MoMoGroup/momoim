#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint32_t date;
} CRPPacketMessageRecordSeek;

__attribute_malloc__
CRPPacketMessageRecordSeek *CRPMessageRecordSeekCast(CRPBaseHeader *base);

int CRPMessageRecordSeekSend(CRPContext context, uint32_t sessionID, uint32_t date);
