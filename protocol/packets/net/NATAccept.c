#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketNETNATAccept *CRPNETNATAcceptCast(CRPBaseHeader *base)
{
    return (CRPPacketNETNATAccept *) base->data;
}

int CRPNETNATAcceptSend(CRPContext context, uint32_t sessionID, uint32_t uid, session_id_t session, uint8_t key[32])
{
    CRPPacketNETNATAccept packet = {
            .uid=uid,
            .session=session
    };
    memcpy(packet.key, key, 32);
    return CRPSend(context,
                   CRP_PACKET_NET_NAT_ACCEPT,
                   sessionID,
                   &packet,
                   sizeof(CRPPacketNETNATAccept)) != -1;
}
