#pragma once

/**
* 通用失败包，表示上一个操作失败
*/
typedef struct
{
    char reason[0];
} CRPPacketStatusFailure;


CRPPacketStatusFailure *CRPStatusFailureCast(CRPBaseHeader *base);

int CRPStatusFailureSend(int sockfd, char *reason);