#pragma once

#include "../base.h"
#include <stdint.h>

CRP_STRUCTURE
{
    uint32_t userid;
    uint32_t sendtime;
    uint16_t message_len;
    char message[0];
} CRPPacketTextMessage;

__attribute_malloc__
CRPPacketTextMessage *CRPTextMessageCast(CRPBaseHeader *base);

int CRPTextMessageSend(int sockfd, uint32_t userid, uint32_t sendtime, uint16_t message_len, char *message);
