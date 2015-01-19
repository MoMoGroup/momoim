#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketNETNATRequest *CRPNETNATRequestCast(CRPBaseHeader *base)
{
    return (CRPPacketNETNATRequest *) base->data;
}

int CRPNETNATRequestSend(CRPContext context,
                         uint32_t sessionID,
                         char key[32],
                         uint32_t uid,
                         uint8_t reason,
                         session_id_t session)
{
    CRPPacketNETNATRequest packet = {
            .reason=reason,
            .uid=uid,
            .session=session
    };
    memcpy(packet.key, key, 32);
    return CRPSend(context,
                   CRP_PACKET_NET_NAT_REQUEST,
                   sessionID,
                   &packet,
                   sizeof(packet)) != -1;
}
