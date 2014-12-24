#pragma once

typedef struct
{
}CRPPacketCrash;

CRPPacketCrash *CRPCrashCast(CRPBaseHeader *base);

int CRPCrastSend(int sockfd);