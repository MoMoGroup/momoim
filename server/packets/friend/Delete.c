#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <datafile/friend.h>
#include "run/user.h"

int ProcessPacketFriendDelete(POnlineUser user, uint32_t session, CRPPacketFriendDelete *packet)
{
    if (user->state == OUS_ONLINE)
    {
        UserFriends *friends = user->info->friends;
        pthread_rwlock_wrlock(user->info->friendsLock);
        UserGroup *group = UserFriendsGroupGet(friends, packet->gid);
        if (!group)
        {
            CRPFailureSend(user->crp, session, ENOENT, "分组未找到");
            return 1;
        }
        if (UserFriendsUserDelete(group, packet->uid))
        {
            UserFriends *peerFriends = UserFriendsGet(packet->uid, NULL, O_RDWR);
            UserGroup *peerGroup = NULL;
            int peerGid = -1;
            for (int i = 0; i < peerFriends->groupCount; ++i)
            {
                peerGroup = peerFriends->groups + i;
                if (UserFriendsUserDelete(peerGroup, user->uid))
                {
                    peerGid = peerGroup->groupId;
                    break;
                }
            }
            UserFriendsDrop(packet->uid);
            if (peerGid != -1)
            {
                OnlineUser *peer = OnlineUserGet(packet->uid);
                if (peer)
                {
                    CRPFriendNotifySend(peer->crp, 0, FNT_FRIEND_DELETE, user->uid, (uint8_t) peerGid, 0);
                    UserDrop(peer);
                }
            }
            CRPFriendNotifySend(user->crp, 0, FNT_FRIEND_DELETE, packet->uid, packet->gid, 0);
            CRPOKSend(user->crp, session);
        }
        else
        {
            CRPFailureSend(user->crp, session, EFAULT, "无法删除用户.");
        }

        pthread_rwlock_unlock(user->info->friendsLock);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}