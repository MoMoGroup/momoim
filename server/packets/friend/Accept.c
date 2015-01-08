#include <asm-generic/errno-base.h>
#include <protocol/friend/Accept.h>
#include <logger.h>
#include "datafile/friend.h"
#include "run/user.h"

int ProcessPacketFriendAccept(POnlineUser user, uint32_t session, CRPPacketFriendAccept *packet)
{
    if (user->status == OUS_ONLINE)
    {

        //添加好友  处理请求者好友列表.
        pthread_rwlock_t *lock;
        UserFriends *reqFriends = UserFriendsGet(packet->uid, &lock);
        pthread_rwlock_wrlock(lock);

        UserGroup *reqDefaultGroup = UserFriendsGroupGet(reqFriends, UGI_DEFAULT),
                *reqPendingGroup = UserFriendsGroupGet(reqFriends, UGI_PENDING);

        if (!reqDefaultGroup)
        {
            reqDefaultGroup = UserFriendsGroupAdd(reqFriends, UGI_DEFAULT, "我的好友");
            log_warning("FriendAccept", "未找到默认好友分组,正在添加分组.\n");
        }
        if (!reqDefaultGroup)
        {
            CRPFailureSend(user->sockfd, session, ENOMEM, "无法获得对方好友分组");
            UserFriendsDrop(packet->uid);
            return 1;
        }

        if (!UserFriendsUserMove(reqPendingGroup, reqDefaultGroup, user->info->uid))
        {
            CRPFailureSend(user->sockfd, session, ENOMEM, "无法添加到对方好友列表中");
            UserFriendsDrop(packet->uid);
            return 1;
        }
        UserFriendsDrop(packet->uid);

        POnlineUser reqUser = OnlineUserGet(packet->uid);
        if (reqUser)
        {
            CRPFriendNotifySend(user->sockfd, session, FNT_FRIEND_MOVE, user->info->uid, UGI_PENDING, UGI_DEFAULT);
            UserDrop(reqUser);
        }

        UserMessage message = {
                .messageType=UMT_FRIEND_ACCEPT,
                .messageLen=0,
                .from=user->info->uid,
                .to=packet->uid,
                .time=time(NULL)
        };
        PostMessage(&message);

        //处理数据包发送者(接受添加好友者)好友列表
        UserFriends *myFriends = user->info->friends;
        lock = user->info->friendsLock;
        pthread_rwlock_wrlock(lock);

        UserGroup *group = UserFriendsGroupGet(myFriends, UGI_DEFAULT);
        if (!group)
        {
            group = UserFriendsGroupAdd(myFriends, UGI_DEFAULT, "我的好友");
            CRPFriendNotifySend(user->sockfd, session, FNT_GROUP_NEW, 0, UGI_DEFAULT, 0);
        }
        if (!group)
        {
            log_warning("FriendAccept",
                        "无法获得用户%u的默认好友分组.\n",
                        user->info->uid
            );
            CRPFailureSend(user->sockfd, session, ENOENT, "无法找到目标分组");
            pthread_rwlock_unlock(lock);
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
            pthread_rwlock_unlock(lock);
            return 1;
        }
        pthread_rwlock_unlock(lock);

        CRPOKSend(user->sockfd, session);
        CRPFriendNotifySend(user->sockfd, 0, FNT_FRIEND_NEW, packet->uid, 0, UGI_DEFAULT);

    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}