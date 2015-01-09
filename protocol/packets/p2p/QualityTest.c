#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketP2PQualityTest *CRPP2PQualityTestCast(CRPBaseHeader *base)
{
    return (CRPPacketP2PQualityTest *) base->data;
}

int CRPP2PQualityTestSend(CRPContext context, uint32_t sessionID, uint16_t seq, const char *pad)
{
    CRPPacketP2PQualityTest packet = {
            .seq = seq
    };
    memcpy(packet.pad, pad, sizeof(packet.pad));
    return CRPSend(context, CRP_PACKET_P2P_QUALITY_TEST, sessionID, &packet, sizeof(CRPPacketP2PQualityTest)) != -1;
}
