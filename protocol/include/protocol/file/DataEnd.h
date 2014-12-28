#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 请求服务器数据
*/

CRP_STRUCTURE
{
    uint8_t code;
    //0 file finished.
    //1 operation canceled.
    //2 file read error.
} CRPPacketFileDataEnd;

__attribute_malloc__
CRPPacketFileDataEnd *CRPFileDataEndCast(CRPBaseHeader *base);

int CRPFileDataEndSend(int sockfd, uint32_t sessionID, uint8_t code);