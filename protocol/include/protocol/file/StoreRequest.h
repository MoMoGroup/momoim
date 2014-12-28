#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include <glob.h>

/**
* 请求服务器数据
*/

CRP_STRUCTURE
{
    size_t length;
    uint8_t type;
    unsigned char key[16];
} CRPPacketFileStoreRequest;

__attribute_malloc__
CRPPacketFileStoreRequest *CRPFileStoreRequestCast(CRPBaseHeader *base);

int CRPFileStoreRequestSend(int sockfd, size_t length, uint8_t type, unsigned char key[16]);