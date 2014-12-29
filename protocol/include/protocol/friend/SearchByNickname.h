#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 请求好友列表
*/

CRP_STRUCTURE {
    char *text;
} CRPPacketFriendSearchByNickname;

__attribute_malloc__
CRPPacketFriendSearchByNickname *CRPFriendSearchByNicknameCast(CRPBaseHeader *base);

int CRPFriendSearchByNicknameSend(int sockfd, uint32_t sessionID, const char *text);