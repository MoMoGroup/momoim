#include <protocol/base.h>
#include <protocol/status/Hello.h>
#include <protocol/CRPPackets.h>


CRPPacketHello *CRPHelloCast(CRPBaseHeader *base)
{
    return (CRPPacketHello *) base->data;
}

int CRPHelloSend(
        CRPContext context,
        uint32_t sessionID,
        uint8_t protocolVersion,
        uint32_t clientVersion,
        uint32_t sessionState,
        uint8_t supportedFeature
)
{
    CRPPacketHello packet = {
            .protocolVersion=protocolVersion,
            .clientVersion=clientVersion,
            .sessionState=sessionState,
            .supportedFeature=supportedFeature
    };

    return CRPSend(context, CRP_PACKET_HELLO, sessionID, &packet, sizeof(packet)) != -1;
}