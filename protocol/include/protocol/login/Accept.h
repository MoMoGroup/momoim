#pragma once
/**
* 服务端接受登陆请求时，回发给客户端的消息
*/
#include <stdint.h>
#include <protocol/base.h>

CRP_STRUCTURE {
    uint32_t uid;
} CRPPacketLoginAccept;

/**
* 登陆包转换
*/
__attribute_malloc__
CRPPacketLoginAccept *CRPLoginAcceptCast(CRPBaseHeader *base);

/**
* 发送登陆包
*/
int CRPLoginAcceptSend(CRPContext context, uint32_t sessionID, uint32_t uid);