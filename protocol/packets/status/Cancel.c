#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketCancel *CRPCancelCast(CRPBaseHeader *base)
{
    return (CRPPacketCancel *) base->data;
}

int CRPCancelSend(CRPContext context, uint32_t sessionID)
{

    return CRPSend(context, CRP_PACKET_CANCEL, sessionID, NULL, 0) != -1;
}