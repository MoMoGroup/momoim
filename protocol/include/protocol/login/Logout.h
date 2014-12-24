#pragma once

typedef struct
{
} CRPPacketLoginLogout;

CRPPacketLoginLogout *CRPLoginLogoutCast(CRPBaseHeader *base);

int CRPLoginLogoutSend(int sockfd);