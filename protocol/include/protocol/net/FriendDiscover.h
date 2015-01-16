#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 好友发现请求
*/
typedef enum
{
    CRPFDR_AUDIO,
    CRPFDR_VEDIO,
    CRPFDR_ONLINE_FILE
} CRPFriendDiscoverReason;
CRP_STRUCTURE
{
    uint8_t gid;
    uint32_t uid;
    uint8_t reason;
    session_id_t session;
} CRPPacketNETFriendDiscover;

__attribute_malloc__
CRPPacketNETFriendDiscover *CRPNETFriendDiscoverCast(CRPBaseHeader *base);

int CRPNETFriendDiscoverSend(CRPContext context,
                             uint32_t sessionID,
                             uint8_t gid,
                             uint32_t uid,
                             uint8_t reason,
                             session_id_t session);