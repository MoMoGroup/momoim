#include <protocol/CRPPackets.h>


CRPPacketNETInetAddress *CRPNETInetAddressCast(CRPBaseHeader *base)
{
    return (CRPPacketNETInetAddress *) base->data;
}

int CRPNETInetAddressSend(CRPContext context, uint32_t sessionID, uint32_t ipv4)
{
    return CRPSend(context, CRP_PACKET_NET_INET_ADDRESS, sessionID, &ipv4, sizeof(ipv4)) != -1;
}