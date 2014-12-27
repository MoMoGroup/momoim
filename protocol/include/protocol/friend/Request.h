#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 请求好友列表
*/

typedef struct
{
} CRPPacketFriendRequest;

__attribute_malloc__
CRPPacketFriendRequest *CRPFriendRequestCast(CRPBaseHeader *base);

int CRPFriendRequestSend(int sockfd);