#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include "imcommon/friends.h"

/**
* 请求好友列表
*/

CRP_STRUCTURE {
    uint32_t uid;
    uint8_t gid;
} CRPPacketFriendDelete;

__attribute_malloc__
CRPPacketFriendDelete *CRPFriendDeleteCast(CRPBaseHeader *base);

int CRPFriendDeleteSend(CRPContext context, uint32_t sessionID, uint32_t uid, uint8_t gid);
