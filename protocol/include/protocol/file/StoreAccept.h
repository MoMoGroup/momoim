#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include <glob.h>

/**
* 服务器接受数据
*/
CRP_STRUCTURE
{
    uint32_t operationId;
} CRPPacketFileStoreAccept;

__attribute_malloc__
CRPPacketFileStoreAccept *CRPFileStoreAcceptCast(CRPBaseHeader *base);

int CRPFileStoreAcceptSend(int sockfd, uint32_t sessionID, uint32_t operationId);