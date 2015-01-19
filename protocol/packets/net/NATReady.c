#include <protocol/CRPPackets.h>

CRPPacketNETNATReady *CRPNETNATReadyCast(CRPBaseHeader *base)
{
    return (CRPPacketNETNATReady *) base->data;
}

int CRPNETNATReadySend(CRPContext context, uint32_t sessionID, uint32_t uid, session_id_t session)
{
    return CRPSend(context,
                   CRP_PACKET_NET_NAT_READY,
                   sessionID,
                   &(CRPPacketNETNATReady) {.session=session, .uid=uid},
                   sizeof(CRPPacketNETNATReady)) != -1;
}
