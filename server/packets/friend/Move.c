#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketFriendMove(POnlineUser user, uint32_t session, CRPPacketFriendMove *packet)
{
    if (user->state == OUS_ONLINE)
    {
        UserFriends *friends = user->info->friends;
        pthread_rwlock_wrlock(user->info->friendsLock);
        UserGroup *groupFrom = UserFriendsGroupGet(friends, packet->fromGid),
                *groupTo = UserFriendsGroupGet(friends, packet->toGid);
        if (!groupFrom || !groupTo)
        {
            CRPFailureSend(user->crp, session, ENOENT, "分组未找到");
            return 1;
        }

        if (!UserFriendsUserAdd(groupTo, packet->uid))//现加后删,一旦删除操作奇葩的失败了.这次失败也有办法还原
        {
            CRPFailureSend(user->crp, session, ENOMEM, "无法添加用户到目标分组");
            return 1;
        }
        UserFriendsUserDelete(groupFrom, packet->uid);
        CRPFriendNotifySend(user->crp, 0, FNT_FRIEND_MOVE, packet->uid, packet->fromGid, packet->toGid);
        CRPOKSend(user->crp, session);
        pthread_rwlock_unlock(user->info->friendsLock);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}