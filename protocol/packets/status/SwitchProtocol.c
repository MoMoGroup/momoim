#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketSwitchProtocol *CRPSwitchProtocolCast(CRPBaseHeader *base)
{
    return (CRPPacketSwitchProtocol *) base->data;
}

int CRPSwitchProtocolSend(CRPContext context, uint32_t sessionID, const char key[32], const char iv[32])
{
    char merge[64];
    memcpy(merge, key, 32);
    memcpy(merge + 32, iv, 32);
    return CRPSend(context, CRP_PACKET_SWITCH_PROTOCOL, sessionID, merge, sizeof(merge)) != -1;
}