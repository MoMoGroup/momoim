#include <protocol/CRPPackets.h>

CRPPacketNETNATRegister *CRPNETNATRegisterCast(CRPBaseHeader *base)
{
    return (CRPPacketNETNATRegister *) base->data;
}

int CRPNETNATRegisterSend(CRPContext context, uint32_t sessionID, uint8_t key[32])
{
    return CRPSend(context, CRP_PACKET_NET_NAT_REGISTER, sessionID, key, 32) != -1;
}
