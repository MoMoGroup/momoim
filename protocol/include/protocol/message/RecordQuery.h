#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    MessageQueryCondition condition;
} CRPPacketMessageRecordQuery;

__attribute_malloc__
CRPPacketMessageRecordQuery *CRPMessageRecordQueryCast(CRPBaseHeader *base);

int CRPMessageRecordQuerySend(CRPContext context, uint32_t sessionID, MessageQueryCondition *condition);
