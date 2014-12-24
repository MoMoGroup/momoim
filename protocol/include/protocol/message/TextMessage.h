#pragma once

#include "../base.h"
#include <stdint.h>

typedef struct
{
    uint32_t userid;
    uint32_t sendtime;
    uint16_t message_len;
    char message[0];
} CRPPacketTextMessage;

CRPPacketTextMessage *CRPTextMessageCast(CRPBaseHeader *base);

int CRPTextMessageSend(int sockfd, uint32_t userid, uint32_t sendtime, uint16_t message_len, char *message);
