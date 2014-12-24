#pragma once

#include <sys/types.h>
#include <stdint.h>

typedef struct
{
    uint32_t MessageSize;
    uint32_t MessageType;
    char MessageData[0];
} SockMessage;

SockMessage *SockRecv(int fd);

int SockSend(int fd, uint32_t msgType, void *data, uint32_t datalen);