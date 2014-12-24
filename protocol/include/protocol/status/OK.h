#pragma once

typedef struct
{
} CRPPacketOK;

CRPPacketOK *CRPOKCast(CRPBaseHeader *base);

int CRPOKSend(int sockfd);