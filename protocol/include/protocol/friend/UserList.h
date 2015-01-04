#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 请求好友列表
*/

CRP_STRUCTURE
{
    uint16_t count;
    uint32_t users[0];
} CRPPacketFriendUserList;

__attribute_malloc__
CRPPacketFriendUserList *CRPFriendUserListCast(CRPBaseHeader *base);

int CRPFriendUserListSend(int sockfd, uint32_t sessionID, uint32_t *uids, uint16_t count);