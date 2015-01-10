#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

#include <string.h>

int ProcessPacketFriendGroupRename(POnlineUser user, uint32_t session, CRPPacketFriendGroupRename *packet)
{
    if (user->status == OUS_ONLINE)
    {
        pthread_rwlock_wrlock(user->info->friendsLock);
        UserGroup *group = UserFriendsGroupGet(user->info->friends, packet->gid);

        if (!group)
        {
            CRPFailureSend(user->sockfd, session, ENOENT, "分组不存在");
        }
        else
        {
            memcpy(group->groupName, packet->groupName, sizeof(group->groupName));
            CRPOKSend(user->sockfd, session);
        }

        pthread_rwlock_unlock(user->info->friendsLock);
    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
}