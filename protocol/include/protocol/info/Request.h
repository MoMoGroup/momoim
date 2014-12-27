#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 客户端请求用户资料
*/

typedef struct
{
    uint32_t uid;
} CRPPacketInfoRequest;

__attribute_malloc__
CRPPacketInfoRequest *CRPInfoRequestCast(CRPBaseHeader *base);

int CRPInfoRequestSend(int sockfd, uint32_t uid);