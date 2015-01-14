#include <protocol/CRPPackets.h>

CRPPacketNETFriendDiscover *CRPNETFriendDiscoverCast(CRPBaseHeader *base)
{
    return (CRPPacketNETFriendDiscover *) base->data;
}

int CRPNETFriendDiscoverSend(CRPContext context, uint32_t sessionID, uint8_t gid, uint32_t uid, uint8_t reason)
{
    CRPPacketNETFriendDiscover packet = {
            .uid=uid,
            .gid=gid,
            .reason=reason
    };
    return CRPSend(context,
                   CRP_PACKET_NET_FRIEND_DISCOVER,
                   sessionID,
                   &packet,
                   sizeof(CRPPacketNETFriendDiscover)) != -1;
}
