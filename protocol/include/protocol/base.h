#pragma once

#include <sys/types.h>
#include <stdint.h>
#include <monetary.h>
#include <glob.h>
#include <mcrypt.h>

typedef __uint16_t CRP_LENGTH_TYPE;
#define CRP_STRUCTURE typedef struct __attribute__ ((packed))
#define NILOBJ(x) ((x *) 0)
typedef uint16_t packet_id_t;
typedef uint32_t session_id_t;
typedef uint32_t user_id_t;

CRP_STRUCTURE
{
    uint32_t magicCode;
    /* 0x464F5573 */
    CRP_LENGTH_TYPE totalLength; //协议包总长度，也用于支持不同协议版本
    packet_id_t packetID;        //包ID，用于区分不同数据包
    session_id_t sessionID;      //消息会话ID
    char data[0];                //包数据部分
} CRPBaseHeader;
typedef struct
{
    int fd;
    MCRYPT sendTd, recvTd;
    char sendKey[32], sendIV[32];
    char recvKey[32], recvIV[32];
    pthread_mutex_t sendLock, recvLock;

} __CRPContext;
typedef __CRPContext *CRPContext;

extern int CRPEncryptEnable(CRPContext context, const char sendKey[32], const char recvKey[32], const char iv[32]);

extern int CRPEncryptTest(const char key[32], const char iv[32]);

extern void CRPEncryptDisable(CRPContext context);

extern CRPContext CRPOpen(int fd);

//*非线程安全*. 不能有多个线程同时使用同一个CRPContext调用该函数!!
//WARNING,This function is thread unsafe!!!
extern int CRPClose(CRPContext);

//用于打包并发送CRP包
extern ssize_t CRPSend(CRPContext context,
                       packet_id_t packetID,
                       session_id_t sessionID,
                       void const *data,
                       CRP_LENGTH_TYPE length);

//用于接收一个CRP包
extern CRPBaseHeader *CRPRecv(CRPContext context);