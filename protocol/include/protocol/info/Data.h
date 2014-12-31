#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 服务端回复用户资料
*/

CRP_STRUCTURE
{
    UserInfo info;
} CRPPacketInfoData;

__attribute_malloc__
CRPPacketInfoData *CRPInfoDataCast(CRPBaseHeader *base);

int CRPInfoDataSend(int sockfd, uint32_t sessionID, UserInfo *info);