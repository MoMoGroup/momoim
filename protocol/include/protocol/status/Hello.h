#pragma once
/**
* 当客户端连接服务器时，发送Hello包以确认协议版本并将客户端状态转换到PENDING_LOGIN
*/
#include "../base.h"
#include <stdint.h>
typedef struct
{
    uint8_t protocolVersion;
    uint32_t clientVersion;
    uint32_t sessionState;
} CRPPacketHello;


CRPPacketHello *CRPHelloCast(CRPBaseHeader *base);

int CRPHelloSend(int sockfd, uint8_t protocolVersion, uint32_t clientVersion, uint32_t sessionState);