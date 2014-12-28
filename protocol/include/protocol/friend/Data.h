#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include "imcommon/friends.h"

/**
* 请求好友列表
*/

CRP_STRUCTURE
{
    unsigned char data[0];
} CRPPacketFriendData;

__attribute_malloc__
CRPPacketFriendData *CRPFriendDataCast(CRPBaseHeader *base);

int CRPFriendDataSend(int sockfd, uint32_t sessionID, UserFriends *friends);