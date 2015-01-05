#include <protocol/CRPPackets.h>


CRPPacketLoginLogout *CRPLoginLogoutCast(CRPBaseHeader *base)
{
    return (CRPPacketLoginLogout *) base->data;
}

int CRPLoginLogoutSend(CRPContext context, uint32_t sessionID)
{
    return CRPSend(context, CRP_PACKET_LOGIN_LOGOUT, sessionID, NULL, 0) != -1;
}