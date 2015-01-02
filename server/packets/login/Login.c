#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <data/auth.h>
#include <logger.h>
#include <unistd.h>
#include <data/user.h>

int ProcessPacketLoginLogin(POnlineUser user, uint32_t session, CRPPacketLogin *packet)
{
    sleep(1);//For Debugger Only
    if (user->status == OUS_PENDING_LOGIN)
    {
        uint32_t uid;
        int ret = AuthUser(packet->username, packet->password, &uid);

        if (ret == 0)
        {
            log_info("Login-Login", "User %s Login failure.\n", packet->username);
            CRPFailureSend(user->sockfd, session, "Login Failure.");
        }
        else
        {
            user->info = UserCreateOnlineInfo(user, uid);
            if (user->info == NULL)
            {
                log_warning("Login-Login", "User %s Login failure. Cannot Create Online Info\n", packet->username);
                CRPFailureSend(user->sockfd, session, "Server Failure.");
            }
            else
            {
                log_info("Login-Login", "User %s (ID:%u) Login Successful.\n", packet->username, uid);
                CRPLoginAcceptSend(user->sockfd, session, uid);
                user->status = OUS_ONLINE;
            }
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
            }
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, "Status Error");
    }
    return 1;
}