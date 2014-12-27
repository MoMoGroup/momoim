#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 客户端请求用户资料
*/

typedef struct
{
    uint32_t uid;
} CRPPacketInfoQuery;

CRPPacketInfoQuery *CRPInfoQueryCast(CRPBaseHeader *base);

int CRPInfoQuerySend(int sockfd, uint32_t uid);