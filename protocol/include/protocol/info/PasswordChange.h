#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include <imcommon/user.h>

/**
* 服务端回复用户资料
*/

CRP_STRUCTURE
{
    char password[16];
} CRPPacketInfoPasswordChange;

__attribute_malloc__
CRPPacketInfoPasswordChange *CRPInfoPasswordChangeCast(CRPBaseHeader *base);

int CRPInfoPasswordChangeSend(CRPContext context, uint32_t sessionID, const char password[16]);
