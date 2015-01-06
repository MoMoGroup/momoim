#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketFriendNotify *CRPFriendNotifyCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendNotify *) base;
}

int CRPFriendNotifySend(CRPContext context, uint32_t sessionID, uint32_t uid, FriendNotifyType type)
{
    CRPPacketFriendNotify packet = {
            .type=(uint8_t) type,
            .uid=uid
    };
    return CRPSend(context, CRP_PACKET_FRIEND_NOTIFY, sessionID, &packet, sizeof(CRPPacketFriendNotify)) != -1;
}