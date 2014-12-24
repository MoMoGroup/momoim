#pragma once

typedef struct
{
    char reason[0];
} CRPPacketLoginFailure;

CRPPacketLoginFailure *CRPLoginLoginFailureCast(CRPBaseHeader *base);

int CRPLoginLoginFailureSend(int sockfd, char *reason);