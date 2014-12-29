#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include <glob.h>

/**
* 请求服务器数据
*/

CRP_STRUCTURE
{
    size_t seq;
    CRP_LENGTH_TYPE length;
    char data[0];
} CRPPacketFileData;

__attribute_malloc__
CRPPacketFileData *CRPFileDataCast(CRPBaseHeader *base);

int CRPFileDataSend(int sockfd, uint32_t sessionID, CRP_LENGTH_TYPE length, size_t seq, char *data);