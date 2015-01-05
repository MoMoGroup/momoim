#include <protocol/base.h>
#include <protocol/status/KeepAlive.h>
#include <protocol/CRPPackets.h>

CRPPacketKeepAlive *CRPKeepAliveCast(CRPBaseHeader *base)
{
    return (CRPPacketKeepAlive *) base->data;
}

int CRPKeepAliveSend(CRPContext context, uint32_t sessionID)
{
    return CRPSend(context, CRP_PACKET_KEEP_ALIVE, sessionID, NULL, 0) != -1;
}