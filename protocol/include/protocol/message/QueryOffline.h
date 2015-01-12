#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
} CRPPacketMessageQueryOffline;

__attribute_malloc__
CRPPacketMessageQueryOffline *CRPMessageQueryOfflineCast(CRPBaseHeader *base);

int CRPMessageQueryOfflineSend(CRPContext context, uint32_t sessionID);
