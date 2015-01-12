#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include <glob.h>

/**
* 请求服务器数据
*/
typedef enum {
    FST_SHARED = 0,   //共享的
    FST_PRIVATE = 1   //私有的
} CRP_PACKET_FILE_STORE_TYPE;
CRP_STRUCTURE {
    size_t length;
    uint8_t type;
    //CRP_PACKET_FILE_STORE_TYPE
    unsigned char key[16];
} CRPPacketFileStoreRequest;

__attribute_malloc__
CRPPacketFileStoreRequest *CRPFileStoreRequestCast(CRPBaseHeader *base);

int CRPFileStoreRequestSend(CRPContext context, uint32_t sessionID, size_t length, uint8_t type, unsigned char key[16]);