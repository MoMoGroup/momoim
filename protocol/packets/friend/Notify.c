#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketFriendNotify *CRPFriendNotifyCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendNotify *) base->data;
}

int CRPFriendNotifySend(CRPContext context, uint32_t sessionID, FriendNotifyType type, uint32_t uid, uint8_t fromGid, uint8_t toGid)
{
    CRPPacketFriendNotify packet = {
            .type=(uint8_t) type,
            .uid=uid,
            .fromGid=fromGid,
            .toGid=toGid
    };
    return CRPSend(context, CRP_PACKET_FRIEND_NOTIFY, sessionID, &packet, sizeof(CRPPacketFriendNotify)) != -1;
}