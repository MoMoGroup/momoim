#include <protocol/friend/Discover.h>
#include <protocol/CRPPackets.h>

CRPPacketFriendDiscover *CRPFriendDiscoverCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendDiscover *) base->data;
}

int CRPFriendDiscoverSend(CRPContext context, uint32_t sessionID, uint8_t gid, uint32_t uid)
{
    CRPPacketFriendDiscover packet = {
            .uid=uid,
            .gid=gid
    };
    return CRPSend(context,
                   CRP_PACKET_FRIEND_DISCOVER,
                   sessionID,
                   &packet,
                   sizeof(CRPPacketFriendDiscover)) != -1;
}
