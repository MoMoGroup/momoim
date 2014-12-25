#pragma once
/**
* 用于确认客户端在线
*/
typedef struct
{
} CRPPacketKeepAlive;

CRPPacketKeepAlive *CRPKeepAliveCast(CRPBaseHeader *base);

int CRPKeepAliveSend(int sockfd);