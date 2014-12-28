#pragma once
/**
* 客户端处于PENDING_LOGIN状态时，向服务器发送登陆请求
*/
#include <stdint.h>
#include <protocol/base.h>

CRP_STRUCTURE
{
    unsigned char password[16];
    char username[0];
} CRPPacketLogin;

/**
* 登陆包转换
*/
__attribute_malloc__
CRPPacketLogin *CRPLoginLoginCast(CRPBaseHeader *base);

/**
* 发送登陆包
*/
int CRPLoginLoginSend(int sockfd, uint32_t sessionID, const char *username, const unsigned char *password);