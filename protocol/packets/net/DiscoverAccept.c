#include <protocol/CRPPackets.h>


CRPPacketNETDiscoverAccept *CRPNETDiscoverAcceptCast(CRPBaseHeader *base)
{
    return (CRPPacketNETDiscoverAccept *) base->data;
}

int CRPNETDiscoverAcceptSend(CRPContext context, uint32_t sessionID, uint32_t uid)
{
    return CRPSend(context, CRP_PACKET_NET_DISCOVER_ACCEPT, sessionID, &uid, sizeof(uid)) != -1;
}