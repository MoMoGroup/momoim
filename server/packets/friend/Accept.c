#include <asm-generic/errno-base.h>
#include <protocol/friend/Accept.h>
#include <data/user.h>
#include <logger.h>
#include "run/user.h"

static int sub(POnlineUser user, uint32_t session, UserFriends *friends)
{
    UserGroup *defaultGroup = UserFriendsGroupGet(friends, UGI_DEFAULT),
            *pendingGroup = UserFriendsGroupGet(friends, UGI_PENDING);

    if (!defaultGroup)
        defaultGroup = UserFriendsGroupAdd(friends, 1, "我的好友");
    if (!defaultGroup)
    {
        CRPFailureSend(user->sockfd, session, ENOMEM, "无法获得对方好友分组");
        return 0;
    }

    if (!UserFriendsUserMove(pendingGroup, defaultGroup, user->info->uid))
    {
        CRPFailureSend(user->sockfd, session, ENOMEM, "无法添加到对方好友列表中");
        return 0;
    }
    return 1;
}

int ProcessPacketFriendAccept(POnlineUser user, uint32_t session, CRPPacketFriendAccept *packet)
{
    if (user->status == OUS_ONLINE)
    {
        pthread_rwlock_t *lock;
        UserFriends *friends = UserFriendsGet(packet->uid, &lock);
        pthread_rwlock_wrlock(lock);
        int succ = sub(user, session, friends);
        UserFriendsDrop(packet->uid);
        if (!succ)
            return 1;
        friends = user->info->friends;
        lock = user->info->friendsLock;
        pthread_rwlock_wrlock(lock);

        UserGroup *group = UserFriendsGroupGet(friends, UGI_DEFAULT);
        if (!group)
        {
            group = UserFriendsGroupAdd(friends, 1, "我的好友");
        }
        if (!group)
        {
            log_warning("FriendAccept",
                    "无法获得用户%u的默认好友分组.无法将用户%u加入到%u的好友列表中\n",
                    user->info->uid,
                    packet->uid,
                    user->info->uid
            );
            UserFriendsDrop(user->info->uid);
            CRPFailureSend(user->sockfd, session, ENOENT, "无法找到目标分组");
            return 1;
        }
        if (!UserFriendsUserAdd(group, packet->uid))
        {
            log_warning("FriendAccept",
                    "无法将用户%u加入到%u的好友列表中\n",
                    packet->uid,
                    user->info->uid
            );
            CRPFailureSend(user->sockfd, session, ENOENT, "无法将好友加入到好友列表中");
            return 1;
        }
        UserFriendsDrop(user->info->uid);
        CRPOKSend(user->sockfd, session);
        CRPFriendNotifySend(user->sockfd, 0, packet->uid, FNT_NEW);
        UserMessage message = {
                .messageType=UMT_NEW_FRIEND,
                .messageLen=0,
                .from=user->info->uid,
                .to=packet->uid
        };
        PostMessage(&message);
    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}