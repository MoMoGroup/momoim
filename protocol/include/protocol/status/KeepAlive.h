#pragma once
/**
* 用于确认客户端在线
*/
typedef struct
{
} CRPPacketKeepAlive;

__attribute_malloc__
CRPPacketKeepAlive *CRPKeepAliveCast(CRPBaseHeader *base);

int CRPKeepAliveSend(int sockfd);