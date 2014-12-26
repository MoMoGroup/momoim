#pragma once

#include <stddef.h>
#include "protocol/login/Login.h"
#include "protocol/login/Accept.h"
#include "protocol/login/Logout.h"

#include "protocol/status/Hello.h"
#include "protocol/status/KeepAlive.h"
#include "protocol/status/OK.h"
#include "protocol/status/Failure.h"
#include "protocol/status/Crash.h"
#include "protocol/message/TextMessage.h"

typedef enum
{
    CRP_PACKET_KEEP_ALIVE = 0,      //心跳包
    CRP_PACKET_HELLO,               //Hello包
    CRP_PACKET_FAILURE,             //通用失败包
    CRP_PACKET_OK,                  //通用接受包
    CRP_PACKET_CRASH = UINT16_MAX,  //崩溃包

    CRP_PACKET_LOGIN__START = 0x10, //登陆类数据包开始
    CRP_PACKET_LOGIN_LOGIN,         //登陆
    CRP_PACKET_LOGIN_ACCEPT,         //登陆
    CRP_PACKET_LOGIN_LOGOUT,        //登出

    CRP_PACKET_MESSAGE__START = 0x20, //消息类数据包开始
    CRP_PACKET_MESSAGE_TEXT,        //文本消息

    CRP_PACKET_ID_MAX = UINT16_MAX  //最大包ID
} CRPPacketIDs;

//数据转换函数映射数组
extern void *(*const PacketsDataCastMap[CRP_PACKET_ID_MAX + 1])(CRPBaseHeader *base);
