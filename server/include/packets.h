#pragma once

#include <user.h>
#include <protocol/CRPPackets.h>
#include <stddef.h>

//数据包处理
int ProcessPacketStatusHello(OnlineUser *user, uint32_t session, CRPPacketHello *packet);

int ProcessPacketStatusKeepAlive(OnlineUser *user, uint32_t session, CRPPacketKeepAlive *packet);

int ProcessPacketStatusOK(OnlineUser *user, uint32_t session, CRPPacketOK *packet);

int ProcessPacketStatusCrash(OnlineUser *user, uint32_t session, CRPPacketCrash *packet);

int ProcessPacketLoginLogin(OnlineUser *user, uint32_t session, CRPPacketLogin *packet);

int ProcessPacketStatusFailure(OnlineUser *user, uint32_t session, CRPPacketFailure *packet);

int ProcessPacketLoginLogout(OnlineUser *user, uint32_t session, CRPPacketLoginLogout *packet);

int ProcessPacketMessageTextMessage(OnlineUser *user, uint32_t session, CRPPacketTextMessage *packet);

int ProcessPacketInfoRequest(OnlineUser *user, uint32_t session, CRPPacketInfoRequest *packet);

int ProcessPacketFriendRequest(OnlineUser *user, uint32_t session, CRPPacketFriendRequest *packet);

int ProcessPacketFileRequest(OnlineUser *user, uint32_t session, CRPPacketFileRequest *packet);

int ProcessPacketFileData(OnlineUser *user, uint32_t session, CRPPacketFileData *packet);

int ProcessPacketFileDataEnd(OnlineUser *user, uint32_t session, CRPPacketFileDataEnd *packet);

int ProcessPacketFileStoreRequest(OnlineUser *user, uint32_t session, CRPPacketFileStoreRequest *packet);

int ProcessPacketLoginRegister(OnlineUser *user, uint32_t session, CRPPacketLoginRegister *packet);

//数据包处理函数映射
extern int(*PacketsProcessMap[CRP_PACKET_ID_MAX + 1])(OnlineUser *user, uint32_t session, void *packet);