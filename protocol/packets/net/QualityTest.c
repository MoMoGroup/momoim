#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketNETQualityTest *CRPNETQualityTestCast(CRPBaseHeader *base)
{
    return (CRPPacketNETQualityTest *) base->data;
}

int CRPNETQualityTestSend(CRPContext context, uint32_t sessionID, uint16_t seq, const char *pad)
{
    CRPPacketNETQualityTest packet = {
            .seq = seq
    };
    memcpy(packet.pad, pad, sizeof(packet.pad));
    return CRPSend(context, CRP_PACKET_NET_QUALITY_TEST, sessionID, &packet, sizeof(CRPPacketNETQualityTest)) != -1;
}
