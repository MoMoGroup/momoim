#pragma once
/**
* 客户端处于PENDING_LOGIN状态时，向服务器发送登陆请求
*/
#include <stdint.h>

typedef struct
{
    char password[16];
    uint8_t username_len;
    char username[0];
} CRPPacketLogin;

/**
* 登陆包转换
*/
CRPPacketLogin *CRPLoginLoginCast(CRPBaseHeader *base);

/**
* 发送登陆包
*/
int CRPLoginLoginSend(int sockfd, uint8_t username_len, char *username, char *password);