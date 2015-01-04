#pragma once

#include "protocol/base.h"
/**
* 取消操作
*/
CRP_STRUCTURE
{
} CRPPacketCancel;

__attribute_malloc__
CRPPacketCancel *CRPCancelCast(CRPBaseHeader *base);

int CRPCancelSend(int sockfd, uint32_t sessionID);