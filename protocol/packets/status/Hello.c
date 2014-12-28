#include <protocol/base.h>
#include <protocol/status/Hello.h>
#include <protocol/CRPPackets.h>


CRPPacketHello *CRPHelloCast(CRPBaseHeader *base)
{
    return (CRPPacketHello *) base->data;
}

int CRPHelloSend(int sockfd, uint32_t sessionID, uint8_t protocolVersion, uint32_t clientVersion, uint32_t sessionState)
{
    CRPPacketHello packet = {
            .protocolVersion=protocolVersion,
            .clientVersion=clientVersion,
            .sessionState=sessionState
    };

    return CRPSend(CRP_PACKET_HELLO, sessionID, &packet, sizeof(packet), sockfd) != -1;
}