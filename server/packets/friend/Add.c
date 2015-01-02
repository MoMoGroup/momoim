#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include <string.h>
#include "run/user.h"

int ProcessPacketFriendAdd(POnlineUser user, uint32_t session, CRPPacketFriendAdd *packet)
{
    //TODO 完成好友添加请求
    if (user->status == OUS_ONLINE)
    {
        pthread_rwlock_rdlock(&user->info->friendsLock);
        UserFriends *friends = user->info->friends;
        UserGroup *pendingGroup = NULL, *group;
        for (int i = 0; i < friends->groupCount; ++i)
        {
            group = friends->groups + i;
            if (group->groupId == UINT8_MAX)
            {
                pendingGroup = group;
            }
            for (int j = 0; j < group->friendCount; ++j)
            {
                if (group->friends[j] == packet->uid)
                {
                    pthread_rwlock_unlock(&user->info->friendsLock);
                    if (group->groupId == UINT8_MAX)
                    {
                        goto sendMsg;
                    }
                    else
                    {
                        CRPFailureSend(user->sockfd, session, EEXIST, "好友已存在");
                        return 1;
                    }
                }
            }
        }
        pthread_rwlock_unlock(&user->info->friendsLock);
        pthread_rwlock_wrlock(&user->info->friendsLock);
        void *ptr;
        if (pendingGroup == NULL)
        {
            ptr = realloc(friends->groups, (friends->groupCount + 1) * sizeof(UserGroup));
            if (ptr == NULL)
            {
                CRPFailureSend(user->sockfd, session, ENOMEM, "无法创建待处理好友分组");
                pthread_rwlock_unlock(&user->info->friendsLock);
                return 1;
            }
            friends->groups = ptr;
            pendingGroup = friends->groups + friends->groupCount;
            ++friends->groupCount;
            pendingGroup->friendCount = 0;
            pendingGroup->friends = (uint32_t *) malloc(sizeof(uint32_t) * pendingGroup->friendCount);
            pendingGroup->groupId = UINT8_MAX;
            memcpy(pendingGroup->groupName, "Pending", 7);
        }
        ptr = realloc(pendingGroup->friends, (pendingGroup->friendCount + 1) * sizeof(uint32_t));
        if (ptr == NULL)
        {
            CRPFailureSend(user->sockfd, session, ENOMEM, "无法添加好友");
            pthread_rwlock_unlock(&user->info->friendsLock);
            return 1;
        }
        pendingGroup->friends = ptr;
        ++pendingGroup->friendCount;
        pendingGroup->friends[pendingGroup->friendCount] = packet->uid;
        pthread_rwlock_unlock(&user->info->friendsLock);
        size_t noteLen = strlen(packet->note);

        sendMsg:
        {
            UserMessage *message = (UserMessage *) malloc(sizeof(UserMessage) + noteLen);
            message->messageType = UMT_NEW_FRIEND;
            message->from = user->info->uid;
            message->to = packet->uid;
            message->messageLen = (uint16_t) noteLen;
            memcpy(message->content, packet->note, noteLen);
            PostMessage(message);
            free(message);
        };
    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}