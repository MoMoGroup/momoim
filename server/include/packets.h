#pragma once

#include <user.h>
#include <protocol/CRPPackets.h>
#include <stddef.h>

//数据包处理
int ProcessPacketStatusHello(OnlineUser *user, CRPPacketHello *packet);

int ProcessPacketStatusKeepAlive(OnlineUser *user, CRPPacketKeepAlive *packet);

int ProcessPacketStatusOK(OnlineUser *user, CRPPacketOK *packet);

int ProcessPacketStatusCrash(OnlineUser *user, CRPPacketCrash *packet);

int ProcessPacketLoginLogin(OnlineUser *user, CRPPacketLogin *packet);

int ProcessPacketStatusFailure(OnlineUser *user, CRPPacketFailure *packet);

int ProcessPacketLoginLogout(OnlineUser *user, CRPPacketLoginLogout *packet);

int ProcessPacketMessageTextMessage(OnlineUser *user, CRPPacketTextMessage *packet);

//数据包处理函数映射
int(*PacketsProcessMap[CRP_PACKET_ID_MAX + 1])(OnlineUser *user, void *packet) = {
        [CRP_PACKET_KEEP_ALIVE]         =(int (*)(OnlineUser *user, void *packet)) ProcessPacketStatusKeepAlive,
        [CRP_PACKET_HELLO]              =(int (*)(OnlineUser *user, void *packet)) ProcessPacketStatusHello,
        [CRP_PACKET_OK]                 =(int (*)(OnlineUser *user, void *packet)) ProcessPacketStatusOK,
        [CRP_PACKET_CRASH]              =(int (*)(OnlineUser *user, void *packet)) ProcessPacketStatusCrash,

        [CRP_PACKET_LOGIN__START]       =(int (*)(OnlineUser *user, void *packet)) NULL,
        [CRP_PACKET_LOGIN_LOGIN]        =(int (*)(OnlineUser *user, void *packet)) ProcessPacketLoginLogin,
        [CRP_PACKET_FAILURE]            =(int (*)(OnlineUser *user, void *packet)) ProcessPacketStatusFailure,
        [CRP_PACKET_LOGIN_LOGOUT]       =(int (*)(OnlineUser *user, void *packet)) ProcessPacketLoginLogout,

        [CRP_PACKET_MESSAGE__START]     =(int (*)(OnlineUser *user, void *packet)) NULL,
        [CRP_PACKET_MESSAGE_TEXT]       =(int (*)(OnlineUser *user, void *packet)) ProcessPacketMessageTextMessage,
};