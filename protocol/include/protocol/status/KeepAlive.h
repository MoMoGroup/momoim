#pragma once

#include "protocol/base.h"
/**
* 用于确认客户端在线
*/
CRP_STRUCTURE {
} CRPPacketKeepAlive;

__attribute_malloc__
CRPPacketKeepAlive *CRPKeepAliveCast(CRPBaseHeader *base);

int CRPKeepAliveSend(CRPContext context, uint32_t sessionID);