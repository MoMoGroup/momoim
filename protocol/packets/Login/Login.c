#include <protocol/base.h>
#include <protocol/login/Login.h>
#include <protocol/packets.h>
#include <string.h>


CRPPacketLogin *CRPLoginLoginCast(CRPBaseHeader *base)
{
    return (CRPPacketLogin *) base->data;
}

int CRPLoginLoginSend(int sockfd, char *reason)
{
    CRPSend(CRP_PACKET_LOGIN_FAILURE, reason, strlen(reason), sockfd);
}