#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 分组重命名
*/

CRP_STRUCTURE {
    uint8_t gid;
    char groupName[64];
} CRPPacketFriendGroupRename;

__attribute_malloc__
CRPPacketFriendGroupRename *CRPFriendFriendGroupRenameCast(CRPBaseHeader *base);

int CRPFriendGroupRenameSend(CRPContext context, uint32_t sessionID, uint8_t gid, const char name[64]);