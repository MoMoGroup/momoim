#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include "imcommon/friends.h"

/**
* 请求好友列表
*/

CRP_STRUCTURE
{
    uint32_t uid;
} CRPPacketFriendAccept;

__attribute_malloc__
CRPPacketFriendAccept *CRPFriendAcceptCast(CRPBaseHeader *base);

int CRPFriendAcceptSend(int sockfd, uint32_t sessionID, uint32_t uid);