#pragma once

#include <stdint.h>

typedef struct
{
    uint16_t username_len;
    char username[0];
    char password[16];
} CRPPacketLogin;


CRPPacketLogin *CRPLoginLoginCast(CRPBaseHeader *base);

int CRPLoginLoginSend(int sockfd, char *reason);