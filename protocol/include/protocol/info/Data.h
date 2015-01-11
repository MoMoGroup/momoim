#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include <imcommon/user.h>

/**
* 服务端回复用户资料
*/

CRP_STRUCTURE
{
    UserInfo info;
    int isOnline;
} CRPPacketInfoData;

__attribute_malloc__
CRPPacketInfoData *CRPInfoDataCast(CRPBaseHeader *base);

int CRPInfoDataSend(CRPContext context, uint32_t sessionID, int isOnline, UserInfo *info);