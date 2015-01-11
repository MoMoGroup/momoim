#pragma once

#include "protocol/base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint16_t seq;
    char pad[1024 - sizeof(uint16_t)];
} CRPPacketNETQualityTest;

__attribute_malloc__
CRPPacketNETQualityTest *CRPNETQualityTestCast(CRPBaseHeader *base);

int CRPNETQualityTestSend(CRPContext context, uint32_t sessionID, uint16_t seq, const char *pad);
