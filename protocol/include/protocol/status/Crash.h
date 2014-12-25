#pragma once

/**
* 在任意一方发生无法恢复的错误时，应向对方发送崩溃包
*/
typedef struct {
} CRPPacketCrash;

/**
* 崩溃包转换
*/
CRPPacketCrash *CRPCrashCast(CRPBaseHeader *base);

/**
* 发送崩溃包
*/
int CRPCrashSend(int sockfd);