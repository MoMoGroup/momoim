#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include "imcommon/friends.h"

/**
* 请求好友列表
*/
typedef enum
{
    FNT_ONLINE,
    FNT_OFFLINE,
    FNT_NEW,
    FNT_REMOVE
} FriendNotifyType;
CRP_STRUCTURE
{
    uint32_t uid;
    uint8_t type;
} CRPPacketFriendNotify;

__attribute_malloc__
CRPPacketFriendNotify *CRPFriendNotifyCast(CRPBaseHeader *base);

int CRPFriendNotifySend(CRPContext context, uint32_t sessionID, uint32_t uid, FriendNotifyType type);