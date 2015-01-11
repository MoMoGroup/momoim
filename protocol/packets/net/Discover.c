#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketNATDiscover *CRPNATDiscoverCast(CRPBaseHeader *base)
{
    return (CRPPacketNATDiscover *) base->data;
}

int CRPNATDiscoverSend(CRPContext context, uint32_t sessionID, uint32_t uid, char key[32])
{
    CRPPacketNATDiscover packet = {
            .uid=uid
    };
    memcpy(packet.key, key, sizeof(packet.key));
    return CRPSend(context, CRP_PACKET_NAT_DISCOVER, uid, &packet, sizeof(CRPPacketNATDiscover)) != -1;
}
