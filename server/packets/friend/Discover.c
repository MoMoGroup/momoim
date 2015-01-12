#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketFriendDiscover(POnlineUser user, uint32_t session, CRPPacketFriendDiscover *packet)
{
    if (user->state == OUS_ONLINE)
    {
        if (packet->gid == UGI_BLACKLIST || packet->gid == UGI_PENDING)
        {
            CRPFailureSend(user->crp, session, EINVAL, "不允许的分组ID");
            return 1;
        }

        pthread_rwlock_rdlock(user->info->friendsLock);
        if (!UserFriendsExist(user->info->friends, packet->gid, packet->gid))
        {
            pthread_rwlock_unlock(user->info->friendsLock);
            CRPFailureSend(user->crp, session, EINVAL, "未在好友分组中找到用户");
            return 1;
        }
        pthread_rwlock_unlock(user->info->friendsLock);

        POnlineUser duser = OnlineUserGet(packet->uid);
        if (!duser || duser->status == UOS_HIDDEN)
        {
            if (duser) UserDrop(duser);
            CRPFailureSend(user->crp, session, ENOENT, "目标用户不在线");
            return 1;
        }
        struct sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);
        if (getpeername(duser->crp->fd, (struct sockaddr *) &addr, &addrLen) == 0)
        {
            CRPNETInetAddressSend(user->crp, session, addr.sin_addr.s_addr);
        }
        else
        {
            CRPFailureSend(user->crp, session, EFAULT, "无法发现该用户");
        }

        UserDrop(duser);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}