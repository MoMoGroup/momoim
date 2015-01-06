#include <protocol/base.h>
#include <protocol/CRPPackets.h>

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
int CRPOKSend(CRPContext context, uint32_t sessionID)
{
    return CRPSend(context, CRP_PACKET_OK, sessionID, NULL, 0) != -1;
}