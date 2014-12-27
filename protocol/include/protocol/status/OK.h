#pragma once

/**
* 通用接受包，表示上一个操作已接受
*/
CRP_STRUCTURE
{
} CRPPacketOK;

__attribute_malloc__
CRPPacketOK *CRPOKCast(CRPBaseHeader *base);

int CRPOKSend(int sockfd);