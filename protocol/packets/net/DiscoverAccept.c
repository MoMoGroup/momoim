#include <protocol/CRPPackets.h>


CRPPacketNETDiscoverAccept *CRPNETDiscoverAcceptCast(CRPBaseHeader *base)
{
    return (CRPPacketNETDiscoverAccept *) base->data;
}

int CRPNETDiscoverAcceptSend(CRPContext context, uint32_t sessionID, uint32_t uid, session_id_t session)
{
    CRPPacketNETDiscoverAccept packet = {
            .uid=uid,
            .session=session
    };
    return CRPSend(context,
                   CRP_PACKET_NET_DISCOVER_ACCEPT,
                   sessionID,
                   &packet,
                   sizeof(CRPPacketNETDiscoverAccept)) != -1;
}