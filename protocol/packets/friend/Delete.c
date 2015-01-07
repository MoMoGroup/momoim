#include <protocol/base.h>
#include <protocol/CRPPackets.h>


CRPPacketFriendDelete *CRPFriendDeleteCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendDelete *) base->data;
}

int CRPFriendDeleteSend(CRPContext context, uint32_t sessionID, uint32_t uid, uint8_t gid)
{
    CRPPacketFriendDelete packet = {
            .gid=gid,
            .uid=uid
    };
    return CRPSend(context, CRP_PACKET_FRIEND_DELETE, sessionID, &packet, sizeof(packet)) != -1;
}
