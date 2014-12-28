#pragma once

#include <sys/types.h>
#include <stdint.h>
#include <monetary.h>
#include <glob.h>

typedef __uint16_t CRP_LENGTH_TYPE;
#define CRP_STRUCTURE typedef struct __attribute__ ((packed))

CRP_STRUCTURE
{
    unsigned int magicCode /* 0x464F5573 */;
    CRP_LENGTH_TYPE totalLength; //协议包总长度，也用于支持不同协议版本
    CRP_LENGTH_TYPE dataLength;  //数据块总长度，用于接下来接收数据块
    uint16_t packetID;           //包ID，用于区分不同数据包
    uint32_t sessionID;          //消息会话ID
    char data[0];                //包数据部分
} CRPBaseHeader;

//用于打包并发送CRP包
//Reentrant
ssize_t CRPSend(uint16_t packetID, uint32_t sessionID, void *data, size_t length, int fd);

//用于接收一个CRP包
//*注意*不要使用多线程同时接收同一个fd的消息!
CRPBaseHeader *CRPRecv(int fd);

//注意!该函数会尝试接收数据校验部分,如果成功接收校验头它也会阻塞当前线程!
//*注意*不要使用多线程同时接收同一个fd的消息!
CRPBaseHeader *CRPRecvNonBlock(int fd);