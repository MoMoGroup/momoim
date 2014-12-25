#include <protocol/base.h>
#include <protocol/status/OK.h>
#include <protocol/CRPPackets.h>
#include <stddef.h>

CRPPacketOK *CRPOKCast(CRPBaseHeader *base)
{
    return (CRPPacketOK *) base->data;
}

int CRPOKSend(int sockfd)
{
    return CRPSend(CRP_PACKET_OK, NULL, 0, sockfd) != 0;
}