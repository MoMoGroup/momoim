#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include "imcommon/friends.h"

/**
* 请求好友列表
*/
typedef enum {
    FNT_FRIEND_ONLINE = 0,
    FNT_FRIEND_OFFLINE,
    FNT_FRIEND_NEW,
    FNT_FRIEND_DELETE,
    FNT_FRIEND_MOVE,
    FNT_GROUP_NEW,
    FNT_GROUP_DELETE,
    FNT_GROUP_RENAME
} FriendNotifyType;
CRP_STRUCTURE {
    uint32_t uid;
    uint8_t type;
    uint8_t fromGid, toGid;
} CRPPacketFriendNotify;

__attribute_malloc__
CRPPacketFriendNotify *CRPFriendNotifyCast(CRPBaseHeader *base);

int CRPFriendNotifySend(CRPContext context, uint32_t sessionID, FriendNotifyType type, uint32_t uid, uint8_t fromGid, uint8_t toGid);