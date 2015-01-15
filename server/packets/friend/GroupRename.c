#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

#include <string.h>

int ProcessPacketFriendGroupRename(POnlineUser user, uint32_t session, CRPPacketFriendGroupRename *packet)
{
    if (user->state == OUS_ONLINE)
    {
        pthread_rwlock_wrlock(user->info->friendsLock);
        UserGroup *group = UserFriendsGroupGet(user->info->friends, packet->gid);

        if (!group)
        {
            CRPFailureSend(user->crp, session, ENOENT, "分组不存在");
        }
        else
        {
            memcpy(group->groupName, packet->groupName, sizeof(group->groupName));
            CRPFriendNotifySend(user->crp, 0, FNT_GROUP_MOVE, 0, packet->gid, 0);
            CRPOKSend(user->crp, session);
        }

        pthread_rwlock_unlock(user->info->friendsLock);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}