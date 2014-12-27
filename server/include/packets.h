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

int ProcessPacketInfoQuery(OnlineUser *user, CRPPacketInfoQuery *packet);

//数据包处理函数映射
extern int(*PacketsProcessMap[CRP_PACKET_ID_MAX + 1])(OnlineUser *user, void *packet);