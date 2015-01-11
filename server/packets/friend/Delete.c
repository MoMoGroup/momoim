#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <datafile/friend.h>
#include "run/user.h"

int ProcessPacketFriendDelete(POnlineUser user, uint32_t session, CRPPacketFriendDelete *packet)
{
    if (user->state == OUS_ONLINE)
    {
        UserFriends *friends = user->info->friends;
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
            for (int i = 0; i < peerFriends->groupCount; ++i)
            {
                peerGroup = peerFriends->groups + i;
                if (UserFriendsUserDelete(peerGroup, packet->uid))
                {
                    break;
                }
                peerGroup = NULL;
            }
            UserFriendsDrop(packet->uid);
            if (peerGroup)
            {
                OnlineUser *peer = OnlineUserGet(packet->uid);
                if (peer)
                {
                    CRPFriendNotifySend(peer->crp, session, FNT_FRIEND_DELETE, user->info->uid, peerGroup->groupId, 0);
                    UserDrop(peer);
                }
            }
            CRPFriendNotifySend(user->crp, session, FNT_FRIEND_DELETE, packet->uid, packet->gid, 0);
            CRPOKSend(user->crp, session);
        }
        else
        {
            CRPFailureSend(user->crp, session, EFAULT, "无法删除用户.");
        }
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}