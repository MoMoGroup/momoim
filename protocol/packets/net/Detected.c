#include <protocol/CRPPackets.h>

CRPPacketNATDetected *CRPNATDetectedCast(CRPBaseHeader *base)
{
    return (CRPPacketNATDetected *) base->data;
}

int CRPNATDetectedSend(CRPContext context, uint32_t sessionID, uint32_t ipv4, uint16_t port)
{
    return CRPSend(context,
                   CRP_PACKET_NET_DETECTED,
                   sessionID,
                   &(CRPPacketNATDetected) {.ipv4=ipv4, .port=port},
                   sizeof(struct sockaddr_in)) != -1;
}
