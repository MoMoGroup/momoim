#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 添加分组
*/

CRP_STRUCTURE
{
    uint8_t gid;
    uint32_t uid;
} CRPPacketFriendDiscover;

__attribute_malloc__
CRPPacketFriendDiscover *CRPFriendDiscoverCast(CRPBaseHeader *base);

int CRPFriendDiscoverSend(CRPContext context, uint32_t sessionID, uint8_t gid, uint32_t uid);