#include <protocol/base.h>
#include <protocol/CRPPackets.h>


CRPPacketFriendRequest *CRPFriendRequestCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendRequest *) base->data;
}

int CRPFriendRequestSend(int sockfd)
{
    return CRPSend(CRP_PACKET_FRIEND_REQUEST, NULL, 0, sockfd) != 0;
}