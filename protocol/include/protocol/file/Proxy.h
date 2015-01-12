#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 数据开始
*/

CRP_STRUCTURE
{
    uint32_t uid;
    uint64_t fileSize;
} CRPPacketFileProxyRequest;

__attribute_malloc__
CRPPacketFileProxyRequest *CRPFileProxyRequestCast(CRPBaseHeader *base);

int CRPFileProxyRequestSend(CRPContext context, uint32_t sessionID, uint32_t uid, uint64_t bandwidthRequest);