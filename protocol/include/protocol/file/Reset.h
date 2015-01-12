#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 请求服务器数据
*/

CRP_STRUCTURE
{
    size_t seq;
} CRPPacketFileReset;

__attribute_malloc__
CRPPacketFileReset *CRPFileResetCast(CRPBaseHeader *base);

int CRPFileResetSend(CRPContext context, uint32_t sessionID, size_t seq);