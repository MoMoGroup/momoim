#include <protocol/CRPPackets.h>

CRPPacketNETDiscoverRefuse *CRPNETDiscoverRefuseCast(CRPBaseHeader *base)
{
    return (CRPPacketNETDiscoverRefuse *) base->data;
}

int CRPNETDiscoverRefuseSend(CRPContext context, uint32_t sessionID, uint32_t uid,session_id_t session)
{
    CRPPacketNETDiscoverRefuse packet={
            .uid=uid,
            .session=session
    };
    return CRPSend(context, CRP_PACKET_NET_DISCOVER_REFUSE, sessionID, &packet, sizeof(packet)) != -1;
}
