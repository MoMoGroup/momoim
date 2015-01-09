#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketP2PDiscover *CRPP2PDiscoverCast(CRPBaseHeader *base)
{
    return (CRPPacketP2PDiscover *) base->data;
}

int CRPP2PDiscoverSend(CRPContext context, uint32_t sessionID, uint32_t uid, char key[32])
{
    CRPPacketP2PDiscover packet = {
            .uid=uid
    };
    memcpy(packet.key, key, sizeof(packet.key));
    return CRPSend(context, CRP_PACKET_P2P_DISCOVER, uid, &packet, sizeof(CRPPacketP2PDiscover)) != -1;
}
