#pragma once

/**
* 通用失败包，表示上一个操作失败
*/
CRP_STRUCTURE
{
    char reason[0];
} CRPPacketFailure;

__attribute_malloc__
CRPPacketFailure *CRPFailureCast(CRPBaseHeader *base);

int CRPFailureSend(int sockfd, uint32_t sessionID, char *reason);