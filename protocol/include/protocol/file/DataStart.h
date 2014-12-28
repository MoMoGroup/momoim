#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 数据开始
*/

CRP_STRUCTURE
{
    uint32_t operationId;
    uint64_t dataLength;
} CRPPacketFileDataStart;

__attribute_malloc__
CRPPacketFileDataStart *CRPFileDataStartCast(CRPBaseHeader *base);

int CRPFileDataStartSend(int sockfd, uint32_t sessionID, uint32_t operationId, uint64_t dataLength);