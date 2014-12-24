#pragma once

#include <stdint.h>

typedef struct
{
    char password[16];
    uint16_t username_len;
    char username[0];
} CRPPacketLogin;


CRPPacketLogin *CRPLoginLoginCast(CRPBaseHeader *base);

int CRPLoginLoginSend(int sockfd, uint8_t username_len, char *username, char *password);