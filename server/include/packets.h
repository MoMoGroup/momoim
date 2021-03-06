#pragma once

#include <protocol/CRPPackets.h>
#include <stddef.h>
#include "run/Structures.h"
#include "run/user.h"

typedef int (*GeneralPacketProcessor)(POnlineUser, uint32_t, void *packet, CRPBaseHeader *);

//数据包处理
//状态消息
int ProcessPacketStatusHello(POnlineUser user, uint32_t session, CRPPacketHello *packet);

int ProcessPacketStatusKeepAlive(POnlineUser user, uint32_t session, CRPPacketKeepAlive *packet);

int ProcessPacketStatusOK(POnlineUser user, uint32_t session, CRPPacketOK *packet);

int ProcessPacketStatusFailure(POnlineUser user, uint32_t session, CRPPacketFailure *packet);

int ProcessPacketStatusCancel(POnlineUser user, uint32_t session, CRPPacketCancel *packet);

int ProcessPacketStatusSwitchProtocol(POnlineUser user, uint32_t session, CRPPacketSwitchProtocol *packet);

int ProcessPacketStatusCrash(POnlineUser user, uint32_t session, CRPPacketCrash *packet);


//登陆登出消息
int ProcessPacketLoginLogin(POnlineUser user, uint32_t session, CRPPacketLogin *packet);

int ProcessPacketLoginRegister(POnlineUser user, uint32_t session, CRPPacketLoginRegister *packet);

int ProcessPacketLoginLogout(POnlineUser user, uint32_t session, CRPPacketLoginLogout *packet);

//消息处理
int ProcessPacketMessageNormal(POnlineUser user, uint32_t session, CRPPacketMessageNormal *packet);

int ProcessPacketMessageQueryOffline(POnlineUser user, uint32_t session, CRPPacketMessageQueryOffline *packet);

int ProcessPacketMessageRecordQuery(POnlineUser user, uint32_t session, CRPPacketMessageRecordQuery *packet);

//信息处理
int ProcessPacketInfoRequest(POnlineUser user, uint32_t session, CRPPacketInfoRequest *packet);

int ProcessPacketInfoData(POnlineUser user, uint32_t session, CRPPacketInfoData *packet);

int ProcessPacketInfoStatusChange(POnlineUser user, uint32_t session, CRPPacketInfoStatusChange *packet);

int ProcessPacketInfoPasswordChange(POnlineUser user, uint32_t session, CRPPacketInfoPasswordChange *packet);

//好友处理
int ProcessPacketFriendRequest(POnlineUser user, uint32_t session, CRPPacketFriendRequest *packet);

int ProcessPacketFriendSearchByNickname(POnlineUser user, uint32_t session, CRPPacketFriendSearchByNickname *packet);

int ProcessPacketFriendAdd(POnlineUser user, uint32_t session, CRPPacketFriendAdd *packet);

int ProcessPacketFriendDelete(POnlineUser user, uint32_t session, CRPPacketFriendDelete *packet);

int ProcessPacketFriendMove(POnlineUser user, uint32_t session, CRPPacketFriendMove *packet);

int ProcessPacketFriendAccept(POnlineUser user, uint32_t session, CRPPacketFriendAccept *packet);

int ProcessPacketFriendGroupAdd(POnlineUser user, uint32_t session, CRPPacketFriendGroupAdd *packet);

int ProcessPacketFriendGroupDelete(POnlineUser user, uint32_t session, CRPPacketFriendGroupDelete *packet);

int ProcessPacketFriendGroupMove(POnlineUser user, uint32_t session, CRPPacketFriendGroupMove *packet);

int ProcessPacketFriendGroupRename(POnlineUser user, uint32_t session, CRPPacketFriendGroupRename *packet);


//文件处理
int ProcessPacketFileRequest(POnlineUser user, uint32_t session, CRPPacketFileRequest *packet);

int ProcessPacketFileData(POnlineUser user, uint32_t session, CRPPacketFileData *packet);

int ProcessPacketFileReset(POnlineUser user, uint32_t session, CRPPacketFileReset *packet);

int ProcessPacketFileDataEnd(POnlineUser user, uint32_t session, CRPPacketFileDataEnd *packet);

int ProcessPacketFileStoreRequest(POnlineUser user, uint32_t session, CRPPacketFileStoreRequest *packet);

//NAT穿透
int ProcessPacketNETNATRegister(POnlineUser user, uint32_t session, CRPPacketNETNATRegister *packet);

int ProcessPacketNETNATRequest(POnlineUser user, uint32_t session, CRPPacketNETNATRequest *packet);

int ProcessPacketNETNATAccept(POnlineUser user, uint32_t session, CRPPacketNETNATAccept *packet);

int ProcessPacketNETNATRefuse(POnlineUser user, uint32_t session, CRPPacketNETNATRefuse *packet);

int ProcessPacketNETNATReady(POnlineUser user, uint32_t session, CRPPacketNETNATReady *packet);

int ProcessPacketNETFriendDiscover(POnlineUser user, uint32_t session, CRPPacketNETFriendDiscover *packet);

int ProcessPacketNetDiscoverRefuse(POnlineUser user, uint32_t session, CRPPacketNETDiscoverRefuse *packet);

int ProcessPacketNetDiscoverAccept(POnlineUser user, uint32_t session, CRPPacketNETDiscoverAccept *packet);