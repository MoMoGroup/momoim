#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketFriendGroupMove(POnlineUser user, uint32_t session, CRPPacketFriendGroupMove *packet)
{

    if (user->state == OUS_ONLINE)
    {
        if (packet->gid == UGI_PENDING)
        {
            CRPFailureSend(user->crp, session, EINVAL, "不允许移动Pending分组");
            return 1;
        }
        if (packet->gid == packet->nextGid)
        {
            CRPFailureSend(user->crp, session, EINVAL, "分组ID相等");
            return 1;
        }
        pthread_rwlock_wrlock(user->info->friendsLock);
        UserGroup *groupPeekup = UserFriendsGroupGet(user->info->friends, packet->gid),
                *groupNextTo = UserFriendsGroupGet(user->info->friends, packet->nextGid);

        if (!groupPeekup || !groupNextTo)
        {
            CRPFailureSend(user->crp, session, ENOENT, "分组不存在");
        }
        else
        {
            UserGroup copyGroup = *groupPeekup;
            if (groupPeekup < groupNextTo)
            {
                for (UserGroup *pGroup = groupPeekup; pGroup < groupNextTo - 1; ++pGroup)
                {
                    *pGroup = *(pGroup + 1);
                }
            }
            else
            {
                for (UserGroup *pGroup = groupPeekup; pGroup > groupNextTo; --pGroup)
                {
                    *pGroup = *(pGroup - 1);
                }
            }
            *groupNextTo = copyGroup;
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