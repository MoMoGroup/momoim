#include <protocol/CRPPackets.h>

CRPPacketNATDetected *CRPNATDetectedCast(CRPBaseHeader *base)
{
    return (CRPPacketNATDetected *) base->data;
}

int CRPNATDetectedSend(CRPContext context, uint32_t sessionID, struct sockaddr_in const *addr)
{
    return CRPSend(context, CRP_PACKET_NET_DETECTED, sessionID, addr, sizeof(struct sockaddr_in)) != -1;
}
