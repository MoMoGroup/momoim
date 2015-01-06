#pragma once

#include "protocol/base.h"
/**
* 当客户端连接服务器时，发送Hello包以确认协议版本并将客户端状态转换到PENDING_LOGIN
*/
#include "../base.h"
#include <stdint.h>

CRP_STRUCTURE
{
    uint8_t protocolVersion;
    uint32_t clientVersion;
    uint32_t sessionState;
    uint8_t supportedFeature;
} CRPPacketHello;


__attribute_malloc__
CRPPacketHello *CRPHelloCast(CRPBaseHeader *base);

int CRPHelloSend(
        CRPContext context,
        uint32_t sessionID,
        uint8_t protocolVersion,
        uint32_t clientVersion,
        uint32_t sessionState,
        uint8_t supportedFeature
);