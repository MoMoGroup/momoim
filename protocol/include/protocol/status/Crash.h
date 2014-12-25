#pragma once

typedef struct {
} CRPPacketCrash;

CRPPacketCrash *CRPCrashCast(CRPBaseHeader *base);

int CRPCrashSend(int sockfd);