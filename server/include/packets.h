#pragma once

#include <protocol/CRPPackets.h>
#include <stddef.h>
#include "run/Structures.h"
#include "run/user.h"

//数据包处理
int ProcessPacketStatusHello(POnlineUser user, uint32_t session, CRPPacketHello *packet);

int ProcessPacketStatusKeepAlive(POnlineUser user, uint32_t session, CRPPacketKeepAlive *packet);

int ProcessPacketStatusOK(POnlineUser user, uint32_t session, CRPPacketOK *packet);

int ProcessPacketStatusFailure(POnlineUser user, uint32_t session, CRPPacketFailure *packet);

int ProcessPacketStatusCrash(POnlineUser user, uint32_t session, CRPPacketCrash *packet);

int ProcessPacketStatusCancel(POnlineUser user, uint32_t session, CRPPacketCancel *packet);

int ProcessPacketLoginLogin(POnlineUser user, uint32_t session, CRPPacketLogin *packet);

int ProcessPacketLoginRegister(POnlineUser user, uint32_t session, CRPPacketLoginRegister *packet);


int ProcessPacketLoginLogout(POnlineUser user, uint32_t session, CRPPacketLoginLogout *packet);

int ProcessPacketMessageText(POnlineUser user, uint32_t session, CRPPacketMessageNormal *packet);

int ProcessPacketInfoRequest(POnlineUser user, uint32_t session, CRPPacketInfoRequest *packet);

int ProcessPacketInfoData(POnlineUser user, uint32_t session, CRPPacketInfoData *packet);

int ProcessPacketFriendRequest(POnlineUser user, uint32_t session, CRPPacketFriendRequest *packet);

int ProcessPacketFriendSearchByNickname(POnlineUser user, uint32_t session, CRPPacketFriendSearchByNickname *packet);

int ProcessPacketFileRequest(POnlineUser user, uint32_t session, CRPPacketFileRequest *packet);

int ProcessPacketFileData(POnlineUser user, uint32_t session, CRPPacketFileData *packet);

int ProcessPacketFriendAdd(POnlineUser user, uint32_t session, CRPPacketFriendAdd *packet);

int ProcessPacketFileReset(POnlineUser user, uint32_t session, CRPPacketFileReset *packet);

int ProcessPacketFileDataEnd(POnlineUser user, uint32_t session, CRPPacketFileDataEnd *packet);

int ProcessPacketFileStoreRequest(POnlineUser user, uint32_t session, CRPPacketFileStoreRequest *packet);


//数据包处理函数映射
extern int(*PacketsProcessMap[CRP_PACKET_ID_MAX + 1])(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header);