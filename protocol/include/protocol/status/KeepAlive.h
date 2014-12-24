#pragma once

typedef struct
{
} CRPPacketKeepAlive;

CRPPacketKeepAlive *CRPKeepAliveCast(CRPBaseHeader *base);

int CRPKeepAliveSend(int sockfd);