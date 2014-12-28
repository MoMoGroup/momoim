#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 请求服务器数据
*/

CRP_STRUCTURE
{
    uint8_t type;
    //0为请求共享区数据,1为请求用户私有数据
    unsigned char key[16];//数据密钥
} CRPPacketFileRequest;

__attribute_malloc__
CRPPacketFileRequest *CRPFileRequestCast(CRPBaseHeader *base);

int CRPFileRequestSend(int sockfd, uint32_t sessionID, uint8_t type, unsigned char *key);