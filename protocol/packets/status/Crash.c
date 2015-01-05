#include <protocol/CRPPackets.h>

CRPPacketCrash *CRPCrashCast(CRPBaseHeader *base)
{
    return (CRPPacketCrash *) base->data;
}

int CRPCrashSend(CRPContext context, uint32_t sessionID)
{
    return CRPSend(context, CRP_PACKET_CRASH, sessionID, NULL, 0) != -1;
}