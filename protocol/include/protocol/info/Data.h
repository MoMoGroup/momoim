#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 服务端回复用户资料
*/

typedef struct
{
    uint32_t uid;
    char nickName[32];
    char sex;
} CRPPacketInfoData;

CRPPacketInfoData *CRPInfoDataCast(CRPBaseHeader *base);

int CRPInfoDataSend(int sockfd, uint32_t uid, char *nick, char sex);