#include <protocol/base.h>
#include <protocol/status/KeepAlive.h>
#include <protocol/packets.h>
#include <stddef.h>

CRPPacketKeepAlive *CRPKeepAliveCast(CRPBaseHeader *base)
{
    return (CRPPacketKeepAlive *) base->data;
}

int CRPKeepAliveSend(int sockfd)
{
    return CRPSend(CRP_PACKET_KEEP_ALIVE, NULL, 0, sockfd) != 0;
}