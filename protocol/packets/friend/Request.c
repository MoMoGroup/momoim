#include <protocol/base.h>
#include <protocol/CRPPackets.h>


CRPPacketFriendRequest *CRPFriendRequestCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendRequest *) base->data;
}

int CRPFriendRequestSend(CRPContext context, uint32_t sessionID)
{
    return CRPSend(context, CRP_PACKET_FRIEND_REQUEST, sessionID, NULL, 0) != -1;
}