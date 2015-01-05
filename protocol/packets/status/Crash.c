#include <protocol/base.h>
#include <protocol/status/Crash.h>
#include <protocol/CRPPackets.h>
#include <stddef.h>

CRPPacketCrash *CRPCrashCast(CRPBaseHeader *base)
{
    return (CRPPacketCrash *) base->data;
}

int CRPCrashSend(int sockfd, uint32_t sessionID)
{
    return CRPSend(CRP_PACKET_CRASH, sessionID, NULL, 0, sockfd) != -1;
}