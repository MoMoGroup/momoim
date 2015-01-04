#pragma once

#include "protocol/base.h"
/**
* 客户端会话被取消
*/
CRP_STRUCTURE
{
    char reason[0];
} CRPPacketKick;

__attribute_malloc__
CRPPacketKick *CRPKickCast(CRPBaseHeader *base);

int CRPKickSend(int sockfd, uint32_t sessionID, const char *reason);