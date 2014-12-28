#include <protocol/base.h>
#include <protocol/CRPPackets.h>


CRPPacketFriendRequest *CRPFriendRequestCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendRequest *) base->data;
}

int CRPFriendRequestSend(int sockfd, uint32_t sessionID)
{
    return CRPSend(CRP_PACKET_FRIEND_REQUEST, sessionID, NULL, 0, sockfd) != 0;
}