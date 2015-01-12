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
    uint8_t fromGid, toGid;
} CRPPacketFriendMove;

__attribute_malloc__
CRPPacketFriendMove *CRPFriendMoveCast(CRPBaseHeader *base);

int CRPFriendMoveSend(CRPContext context, uint32_t sessionID, uint32_t uid, uint8_t fromGid, uint8_t toGid);