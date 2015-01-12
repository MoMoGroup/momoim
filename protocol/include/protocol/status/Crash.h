#pragma once

#include "protocol/base.h"

/**
* 在任意一方发生无法恢复的错误时，应向对方发送崩溃包
*/
CRP_STRUCTURE {
} CRPPacketCrash;

/**
* 崩溃包转换
*/
__attribute_malloc__
CRPPacketCrash *CRPCrashCast(CRPBaseHeader *base);

/**
* 发送崩溃包
*/
int CRPCrashSend(CRPContext context, uint32_t sessionID);