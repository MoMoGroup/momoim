#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 数据结束
*/
typedef enum {
    FEC_OK = 0,     //文件正常结束
    FEC_CANCELED,   //操作被取消
    FEC_READ_ERROR, //文件读取错误
    FEC_WRITE_ERROR, //文件写入错误
    FEC_ALREADY_EXISTS//文件已存在
} CRP_PACKET_FILE_END_CODE;
CRP_STRUCTURE {
    uint8_t code;//CRP_PACKET_FILE_END_CODE
} CRPPacketFileDataEnd;

__attribute_malloc__
CRPPacketFileDataEnd *CRPFileDataEndCast(CRPBaseHeader *base);

int CRPFileDataEndSend(CRPContext context, uint32_t sessionID, uint8_t code);