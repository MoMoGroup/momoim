#pragma once

#include "protocol/base.h"
/**
* 切换协议.该数据包用于将网络数据切换到加密模式
*/
CRP_STRUCTURE {
    char key[32], iv[32];
} CRPPacketSwitchProtocol;

__attribute_malloc__
CRPPacketSwitchProtocol *CRPSwitchProtocolCast(CRPBaseHeader *base);

int CRPSwitchProtocolSend(CRPContext context, uint32_t sessionID, const char key[32], const char iv[32]);