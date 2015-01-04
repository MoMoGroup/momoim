#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <stdlib.h>
#include <data/user.h>
#include <asm-generic/errno.h>

int ProcessPacketInfoRequest(POnlineUser user, uint32_t session, CRPPacketInfoRequest *packet)
{
    if (user->status == OUS_ONLINE)
    {
        UserInfo *info = UserInfoGet(packet->uid);
        if (info == NULL)
        {
            CRPFailureSend(user->sockfd, session, ENODATA, "无法读取用户资料");
        }
        else
        {
            POnlineUser duser = OnlineUserGet(packet->uid);
            CRPInfoDataSend(user->sockfd, session, duser && duser->status == OUS_ONLINE, info);
            OnlineUserDrop(duser);
            UserInfoFree(info);
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}