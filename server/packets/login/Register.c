#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <data/auth.h>
#include <data/user.h>
#include <string.h>
#include <asm-generic/errno-base.h>

int ProcessPacketLoginRegister(POnlineUser user, uint32_t session, CRPPacketLoginRegister *packet)
{
    if (user->status == OUS_PENDING_LOGIN)
    {
        uint32_t uid;
        uid = AuthRegister(packet->username, packet->password);
        if (uid > 0)
        {
            UserCreateDirectory(uid);
            UserInfo *info = UserInfoGet(uid);
            bzero(info->nickName, sizeof(info->nickName));
            memcpy(info->nickName, packet->nickname,
                   packet->nicknameLength > sizeof(info->nickName) ? sizeof(info->nickName) : packet->nicknameLength);
            UserInfoSave(uid, info);
            UserInfoFree(info);
            CRPOKSend(user->sockfd, session);
        }
        else
        {
            CRPFailureSend(user->sockfd, session, EEXIST, "用户名已存在");
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}