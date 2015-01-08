#include <protocol/base.h>
#include "protocol/CRPPackets.h"

CRPPacketMessageQueryOffline *CRPMessageQueryOfflineCast(CRPBaseHeader *base)
{
    return (CRPPacketMessageQueryOffline *) base->data;
}

int CRPMessageQueryOfflineSend(CRPContext context, uint32_t sessionID)
{
    return CRPSend(context, CRP_PACKET_MESSAGE_QUERY_OFFLINE, sessionID, NULL, 0) != -1;
}
