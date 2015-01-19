#include <protocol/CRPPackets.h>

CRPPacketNETNATRefuse *CRPNETNATRefuseCast(CRPBaseHeader *base)
{
    return (CRPPacketNETNATRefuse *) base->data;
}

int CRPNETNATRefuseSend(CRPContext context, uint32_t sessionID, uint32_t uid, session_id_t session)
{
    return CRPSend(context,
                   CRP_PACKET_NET_NAT_REFUSE,
                   sessionID,
                   &(CRPPacketNETNATRefuse) {
                           .uid=uid,
                           .session=session
                   },
                   sizeof(CRPPacketNETNATRefuse)) != -1;
}
