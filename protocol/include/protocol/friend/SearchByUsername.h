#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 请求好友列表
*/

CRP_STRUCTURE {
    char *text;
} CRPPacketFriendSearchByUsername;

__attribute_malloc__
CRPPacketFriendSearchByNickname *CRPFriendSearchByUsernameCast(CRPBaseHeader *base);

int CRPFriendSearchByUsernameSend(int sockfd, uint32_t sessionID, const char *text);