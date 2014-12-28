#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 请求服务器数据
*/

CRP_STRUCTURE
{
} CRPPacketFileDataEnd;

__attribute_malloc__
CRPPacketFileDataEnd *CRPFileDataEndCast(CRPBaseHeader *base);

int CRPFileDataEndSend(int sockfd);