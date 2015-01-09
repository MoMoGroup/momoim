#pragma once

#include "protocol/base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint16_t seq;
    char pad[1024 - sizeof(uint16_t)];
} CRPPacketP2PQualityTest;

__attribute_malloc__
CRPPacketP2PQualityTest *CRPP2PQualityTestCast(CRPBaseHeader *base);

int CRPP2PQualityTestSend(CRPContext context, uint32_t sessionID, uint16_t seq, const char *pad);
