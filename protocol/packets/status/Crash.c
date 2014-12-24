#include <protocol/base.h>
#include <protocol/status/Crash.h>
#include <protocol/packets.h>
#include <stddef.h>

CRPPacketCrash *CRPCrashCast(CRPBaseHeader *base) {
    return (CRPPacketCrash *) base->data;
}

int CRPCrastSend(int sockfd) {
    return CRPSend(CRP_PACKET_CRASH, NULL, 0, sockfd) != 0;
}