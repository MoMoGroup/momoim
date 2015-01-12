#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"
#include <errno.h>

int ProcessPacketFriendGroupAdd(POnlineUser user, uint32_t session, CRPPacketFriendGroupAdd *packet)
{

    if (user->state == OUS_ONLINE)
    {
        pthread_rwlock_wrlock(user->info->friendsLock);

        if (UserFriendsGroupAdd(user->info->friends, packet->gid, packet->groupName))
        {
            CRPFriendNotifySend(user->crp, 0, FNT_GROUP_NEW, 0, packet->gid, 0);
            CRPOKSend(user->crp, session);
        }
        else
        {
            CRPFailureSend(user->crp, session, errno, "无法创建分组");
        }

        pthread_rwlock_unlock(user->info->friendsLock);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}