#pragma once

#include "../base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint32_t uid;
    char key[32];
} CRPPacketNATDiscover;

__attribute_malloc__
CRPPacketNATDiscover *CRPNATDiscoverCast(CRPBaseHeader *base);

int CRPNATDiscoverSend(CRPContext context, uint32_t sessionID, uint32_t uid, char key[32]);
