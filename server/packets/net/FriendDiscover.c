#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketNETFriendDiscover(POnlineUser user, uint32_t session, CRPPacketNETFriendDiscover *packet)
{
    if (user->state == OUS_ONLINE)
    {
        if (packet->gid == UGI_BLACKLIST || packet->gid == UGI_PENDING)
        {
            CRPFailureSend(user->crp, session, EINVAL, "不允许的分组ID");
            return 1;
        }

        pthread_rwlock_rdlock(user->info->friendsLock);
        if (!UserFriendsExist(user->info->friends, packet->gid, packet->uid))
        {
            pthread_rwlock_unlock(user->info->friendsLock);
            CRPFailureSend(user->crp, session, EINVAL, "未在好友分组中找到用户");
            return 1;
        }
        pthread_rwlock_unlock(user->info->friendsLock);

        POnlineUser duser = OnlineUserGet(packet->uid);
        if (!duser)
        {
            CRPFailureSend(user->crp, session, ENOENT, "目标用户不在线");
            return 1;
        }

        CRPNETFriendDiscoverSend(duser->crp, 0, UGI_BLACKLIST, packet->uid, packet->reason);
        UserDrop(duser);
        CRPOKSend(user->crp, session);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}