#include <protocol/base.h>
#include <protocol/login/LoginFailure.h>
#include <protocol/packets.h>
#include <string.h>


CRPPacketLoginFailure *CRPLoginLoginFailureCast(CRPBaseHeader *base)
{
    return (CRPPacketLoginFailure *) base->data;
}

int CRPLoginLoginFailureSend(int sockfd, char *reason)
{

    return CRPSend(CRP_PACKET_LOGIN_FAILURE, reason, strlen(reason), sockfd) != -1;
}