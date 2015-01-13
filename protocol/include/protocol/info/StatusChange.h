#pragma once

#include <protocol/base.h>
#include <stdint.h>
#include <stddef.h>

/**
* 更变在线状态
*/

CRP_STRUCTURE
{
    uint8_t hidden;
} CRPPacketInfoStatusChange;

__attribute_malloc__
CRPPacketInfoStatusChange *CRPInfoStatusChangeCast(CRPBaseHeader *base);

int CRPInfoStatusChangeSend(CRPContext context, uint32_t sessionID, uint8_t hidden);
