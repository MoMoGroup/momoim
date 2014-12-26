#pragma once

/**
* 通用失败包，表示上一个操作失败
*/
typedef struct
{
    char reason[0];
} CRPPacketFailure;


CRPPacketFailure *CRPFailureCast(CRPBaseHeader *base);

int CRPFailureSend(int sockfd, char *reason);