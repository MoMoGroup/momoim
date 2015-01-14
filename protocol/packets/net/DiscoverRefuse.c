#include <protocol/CRPPackets.h>

CRPPacketNETDiscoverRefuse *CRPNETDiscoverRefuseCast(CRPBaseHeader *base)
{
    return (CRPPacketNETDiscoverRefuse *) base->data;
}

int CRPNETDiscoverRefuseSend(CRPContext context, uint32_t sessionID, uint32_t uid)
{
    return CRPSend(context, CRP_PACKET_NET_DISCOVER_REFUSE, sessionID, &uid, sizeof(uid)) != -1;
}
