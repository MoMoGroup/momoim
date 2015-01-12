#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include <string.h>
#include "run/user.h"

int ProcessPacketFriendAdd(POnlineUser user, uint32_t session, CRPPacketFriendAdd *packet)
{
    if (user->state == OUS_ONLINE)
    {
        pthread_rwlock_rdlock(user->info->friendsLock);
        UserFriends *friends = user->info->friends;
        UserGroup *pendingGroup = NULL, *group;//搜索好友请求等待分组并确定是否已经有过添加请求
        for (int i = 0; i < friends->groupCount; ++i)
        {
            group = friends->groups + i;
            if (group->groupId == UINT8_MAX)//好友请求等待分组标志号为UINT8_MAX.找到分组
            {
                pendingGroup = group;
            }
            for (int j = 0; j < group->friendCount; ++j)//在分组下搜索好友是否已存在
            {
                if (group->friends[j] == packet->uid)
                {
                    if (group->groupId == UINT8_MAX)//如果好友请求已发出正在等待处理
                    {
                        goto sendNotifyMessage;
                        CRPFailureSend(user->crp, session, EAGAIN, "已有添加好友请求");
                    }
                    else
                    {
                        CRPFailureSend(user->crp, session, EEXIST, "好友已存在");
                    }
                    pthread_rwlock_unlock(user->info->friendsLock);
                    return 1;
                }
            }
        }
        pthread_rwlock_unlock(user->info->friendsLock);//要对好友分组进行修改了.这里换锁
        pthread_rwlock_wrlock(user->info->friendsLock);

        if (!pendingGroup)//如果未找到好友请求等待分组,则创建一个新的分组.
            pendingGroup = UserFriendsGroupAdd(user->info->friends, UINT8_MAX, "Pending");

        if (!pendingGroup)
        {
            CRPFailureSend(user->crp, session, ENOMEM, "无法创建待处理好友分组");
            pthread_rwlock_unlock(user->info->friendsLock);
            return 1;
        }

        if (!UserFriendsUserAdd(pendingGroup, packet->uid))
        {
            CRPFailureSend(user->crp, session, ENOMEM, "无法添加好友");
            pthread_rwlock_unlock(user->info->friendsLock);
            return 1;
        }
        CRPFriendNotifySend(user->crp, session, FNT_FRIEND_NEW, packet->uid, 0, UGI_PENDING);

        sendNotifyMessage:
        pthread_rwlock_unlock(user->info->friendsLock);
        CRPOKSend(user->crp, session);

        size_t noteLen = strlen(packet->note);
        UserMessage *message = (UserMessage *) malloc(sizeof(UserMessage) + noteLen);
        message->messageType = UMT_NEW_FRIEND;
        message->from = user->uid;
        message->to = packet->uid;
        message->messageLen = (uint16_t) noteLen;
        time(&message->time);
        memcpy(message->content, packet->note, noteLen);
        PostMessage(message);
        free(message);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}