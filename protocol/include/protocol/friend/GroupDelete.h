#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 分组删除请求
*/

CRP_STRUCTURE
{
    uint8_t gid;
} CRPPacketFriendGroupDelete;

__attribute_malloc__
CRPPacketFriendGroupDelete *CRPFriendFriendGroupDeleteCast(CRPBaseHeader *base);

int CRPFriendGroupDeleteSend(CRPContext context, uint32_t sessionID, uint8_t gid);