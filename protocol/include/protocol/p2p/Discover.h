#pragma once

#include "../base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint32_t uid;
    char key[32];
} CRPPacketP2PDiscover;

__attribute_malloc__
CRPPacketP2PDiscover *CRPP2PDiscoverCast(CRPBaseHeader *base);

int CRPP2PDiscoverSend(CRPContext context, uint32_t sessionID, uint32_t uid, char key[32]);
