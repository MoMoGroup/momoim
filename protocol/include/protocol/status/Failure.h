#pragma once

#include "protocol/base.h"

/**
* 通用失败包，表示操作失败
*/
CRP_STRUCTURE
{
    uint8_t code;
    char reason[0];
} CRPPacketFailure;

__attribute_malloc__
CRPPacketFailure *CRPFailureCast(CRPBaseHeader *base);

int CRPFailureSend(CRPContext context, uint32_t sessionID, uint8_t code, char *reason);