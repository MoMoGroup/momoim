#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include <imcommon/user.h>

/**
* 服务端回复用户资料
*/

CRP_STRUCTURE
{
    char oldPwd[16], newPwd[16];
} CRPPacketInfoPasswordChange;

__attribute_malloc__
CRPPacketInfoPasswordChange *CRPInfoPasswordChangeCast(CRPBaseHeader *base);

int CRPInfoPasswordChangeSend(CRPContext context, uint32_t sessionID, const char oldPwd[16], const char newPwd[16]);
