#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 请求好友列表
*/

CRP_STRUCTURE {
} CRPPacketFriendRequest;

__attribute_malloc__
CRPPacketFriendRequest *CRPFriendRequestCast(CRPBaseHeader *base);

int CRPFriendRequestSend(CRPContext context, uint32_t sessionID);