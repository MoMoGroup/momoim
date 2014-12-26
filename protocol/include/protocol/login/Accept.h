#pragma once
/**
* 服务端接受登陆请求时，回发给客户端的消息
*/
#include <stdint.h>

typedef struct
{
    uint32_t uid;
} CRPPacketLoginAccept;

/**
* 登陆包转换
*/
CRPPacketLoginAccept *CRPLoginAcceptCast(CRPBaseHeader *base);

/**
* 发送登陆包
*/
int CRPLoginAcceptSend(int sockfd, uint32_t uid);