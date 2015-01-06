#pragma once

#include "protocol/base.h"
/**
* 通用接受包，表示上一个操作已接受
*/
CRP_STRUCTURE
{
} CRPPacketOK;

__attribute_malloc__
CRPPacketOK *CRPOKCast(CRPBaseHeader *base);

int CRPOKSend(CRPContext context, uint32_t sessionID);