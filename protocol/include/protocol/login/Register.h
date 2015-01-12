#pragma once
/**
* 服务端接受登陆请求时，回发给客户端的消息
*/
#include <stdint.h>
#include <protocol/base.h>

CRP_STRUCTURE
{
    uint8_t usernameLength;
    uint8_t nicknameLength;
    unsigned char password[16];
    char *username;
    char *nickname;
} CRPPacketLoginRegister;

/**
* 注册请求
*/
__attribute_malloc__
CRPPacketLoginRegister *CRPLoginRegisterCast(CRPBaseHeader *base);

/**
* 发送注册请求
*/
int CRPLoginRegisterSend(
        CRPContext context,
        uint32_t sessionID,
        const char *username,
        const unsigned char *password,
        const char *nickname
);