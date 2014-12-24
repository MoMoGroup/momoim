#include <protocol/base.h>
#include <protocol/login/Logout.h>
#include <protocol/packets.h>
#include <string.h>


CRPPacketLoginLogout *CRPLoginLogoutCast(CRPBaseHeader *base)
{
    return (CRPPacketLoginLogout *) base->data;
}

int CRPLoginLogoutSend(int sockfd)
{
    return CRPSend(CRP_PACKET_LOGIN_LOGOUT, NULL, 0, sockfd) != -1;
}