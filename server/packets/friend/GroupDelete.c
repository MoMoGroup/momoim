#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"
#include <errno.h>

int ProcessPacketFriendGroupDelete(POnlineUser user, uint32_t session, CRPPacketFriendGroupDelete *packet)
{

    if (user->status == OUS_ONLINE) {
        pthread_rwlock_wrlock(user->info->friendsLock);

        if (UserFriendsGroupDelete(user->info->friends, packet->gid)) {
            CRPFriendNotifySend(user->sockfd, session, FNT_GROUP_DELETE, 0, packet->gid, 0);
            CRPOKSend(user->sockfd, session);
        }
        else {
            CRPFailureSend(user->sockfd, session, errno, "无法创建分组");
        }

        pthread_rwlock_unlock(user->info->friendsLock);
    }
    else {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}