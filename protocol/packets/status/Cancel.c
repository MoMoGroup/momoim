#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketCancel *CRPCancelCast(CRPBaseHeader *base)
{
    return (CRPPacketCancel *) base->data;
}

int CRPCancelSend(int sockfd, uint32_t sessionID)
{

    return CRPSend(CRP_PACKET_CANCEL, sessionID, NULL, 0, sockfd) != -1;
}