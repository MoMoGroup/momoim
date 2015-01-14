#include <protocol/CRPPackets.h>

CRPPacketNATDiscover *CRPNATDiscoverCast(CRPBaseHeader *base)
{
    return (CRPPacketNATDiscover *) base->data;
}

int CRPNATDiscoverSend(CRPContext context, uint32_t sessionID, char key[32])
{
    return CRPSend(context, CRP_PACKET_NET_NAT_DISCOVER, sessionID, key, sizeof(CRPPacketNATDiscover)) != -1;
}
