#pragma once

/**
* 通用接受包，表示上一个操作已接受
*/
typedef struct
{
} CRPPacketOK;

CRPPacketOK *CRPOKCast(CRPBaseHeader *base);

int CRPOKSend(int sockfd);