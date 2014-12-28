#include <protocol/base.h>
#include <protocol/status/KeepAlive.h>
#include <protocol/CRPPackets.h>
#include <stddef.h>

CRPPacketKeepAlive *CRPKeepAliveCast(CRPBaseHeader *base)
{
    return (CRPPacketKeepAlive *) base->data;
}

int CRPKeepAliveSend(int sockfd, uint32_t sessionID)
{
    return CRPSend(CRP_PACKET_KEEP_ALIVE, sessionID, NULL, 0, sockfd) != 0;
}