#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include "imcommon/friends.h"

/**
* 请求好友列表
*/

CRP_STRUCTURE
{
    uint32_t uid;
    char note[0];
} CRPPacketFriendAdd;

__attribute_malloc__
CRPPacketFriendAdd *CRPFriendAddCast(CRPBaseHeader *base);

int CRPFriendAddSend(int sockfd, uint32_t sessionID, uint32_t uid, const char *note);