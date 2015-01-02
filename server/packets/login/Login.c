#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <data/auth.h>
#include <logger.h>
#include <data/user.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <unistd.h>

int ProcessPacketLoginLogin(POnlineUser user, uint32_t session, CRPPacketLogin *packet)
{
#ifndef NDEBUG
    sleep(1);//For Debugger
#endif
    if (user->status == OUS_PENDING_LOGIN)
    {
        uint32_t uid;
        int ret = AuthUser(packet->username, packet->password, &uid);

        if (ret == 0)
        {
            log_info("Login-Login", "User %s Login failure.\n", packet->username);
            CRPFailureSend(user->sockfd, session, EFAULT, "用户名或密码不正确");
        }
        else
        {
            OnlineUser *duser = OnlineUserGet(uid);
            if (duser)
            {
                CRPKickSend(duser->sockfd, 0, "另一用户已经登陆");
                OnlineUserDelete(duser);
            }
            if (!UserCreateOnlineInfo(user, uid))
            {
                log_warning("Login-Login", "User %s Login failure. Cannot Create Online Info\n", packet->username);
                CRPFailureSend(user->sockfd, session, ENODATA, "服务器内部错误");
            }
            else
            {
                log_info("Login-Login", "User %s (ID:%u) Login Successful.\n", packet->username, uid);
                CRPLoginAcceptSend(user->sockfd, session, uid);
                user->status = OUS_ONLINE;
            }
            //测试数据导入,开始
            if (uid == 10000 || uid == 10001)
            {
                uint32_t userFriends1[2];
                if (uid == 10000)
                {
                    userFriends1[0] = 10000;
                    userFriends1[1] = 10001;
                }
                else
                {
                    userFriends1[0] = 10001;
                    userFriends1[1] = 10000;
                }

                UserGroup group[3] = {
                        {
                                .groupId=0,
                                .groupName="我的好友",
                                .friendCount=2,
                                .friends=userFriends1
                        },
                        {
                                .groupId=255,
                                .groupName="黑名单",
                                .friendCount=0,
                                .friends=NULL
                        },
                };
                UserFriends friends =
                        {
                                .groupCount=2,
                                .groups=group
                        };
                UserSaveFriendsFile(uid, &friends);
                UserFreeFriends(user->info->friends);
                user->info->friends = UserGetFriends(uid);
            }
            pthread_rwlock_rdlock(&user->info->friendsLock);
            //测试数据导入结束
            for (int i = 0; i < user->info->friends->groupCount; ++i)
            {
                UserGroup *group = user->info->friends->groups + i;
                for (int j = 0; j < group->friendCount; ++j)
                {
                    duser = OnlineUserGet(group->friends[j]);
                    if (duser)
                    {
                        if (duser->status == OUS_ONLINE)
                        {
                            CRPFriendNotifySend(duser->sockfd, 0, uid, FNT_ONLINE);
                        }
                        OnlineUserDrop(duser);
                    }
                }
            }
            pthread_rwlock_unlock(&user->info->friendsLock);
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}