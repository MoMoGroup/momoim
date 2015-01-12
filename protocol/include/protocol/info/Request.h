#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 客户端请求用户资料
*/

CRP_STRUCTURE {
    uint32_t uid;
} CRPPacketInfoRequest;

__attribute_malloc__
CRPPacketInfoRequest *CRPInfoRequestCast(CRPBaseHeader *base);

int CRPInfoRequestSend(CRPContext context, uint32_t sessionID, uint32_t uid);