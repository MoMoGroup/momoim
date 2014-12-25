#include <protocol/base.h>
#include <protocol/status/OK.h>
#include <protocol/CRPPackets.h>
#include <stddef.h>

/**
* 将基本包转换为CRPPacketOK包
*/
CRPPacketOK *CRPOKCast(CRPBaseHeader *base)
{
    return (CRPPacketOK *) base->data;
}

/**
* 向目标socket发送OK消息
*/
int CRPOKSend(int sockfd)
{
    return CRPSend(CRP_PACKET_OK, NULL, 0, sockfd) != 0;
}