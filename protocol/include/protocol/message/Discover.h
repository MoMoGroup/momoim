#pragma once

#include "../base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint32_t uid;
    char key[32];
} CRPPacketMessageDiscover;

__attribute_malloc__
CRPPacketMessageDiscover *CRPMessageDiscoverCast(CRPBaseHeader *base);

int CRPMessageDiscoverSend(CRPContext context, uint32_t sessionID, uint32_t uid, char key[32]);
