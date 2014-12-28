#include <protocol/base.h>
#include <protocol/CRPPackets.h>


CRPPacketLoginAccept *CRPLoginAcceptCast(CRPBaseHeader *base)
{
    return (CRPPacketLoginAccept *) base->data;
}

/**
* 发送登陆包
*/
int CRPLoginAcceptSend(int sockfd, uint32_t uid)
{
    return CRPSend(CRP_PACKET_LOGIN_ACCEPT, 0, &uid, sizeof(uid), sockfd) != 0;
}