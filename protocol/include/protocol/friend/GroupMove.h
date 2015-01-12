#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 分组移动
*/

CRP_STRUCTURE
{
    uint8_t gid, nextGid;
} CRPPacketFriendGroupMove;

__attribute_malloc__
CRPPacketFriendGroupMove *CRPFriendFriendGroupMoveCast(CRPBaseHeader *base);

int CRPFriendGroupMoveSend(CRPContext context, uint32_t sessionID, uint8_t gid, uint8_t nextGid);