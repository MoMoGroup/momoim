#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 添加分组
*/

CRP_STRUCTURE {
    uint8_t gid;
    char groupName[64];
} CRPPacketFriendGroupAdd;

__attribute_malloc__
CRPPacketFriendGroupAdd *CRPFriendFriendGroupAddCast(CRPBaseHeader *base);

int CRPFriendGroupAddSend(CRPContext context, uint32_t sessionID, uint8_t gid, const char name[64]);