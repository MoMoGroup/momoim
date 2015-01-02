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
            /*if (uid == 10000)
            {
                uint32_t userFriends1[] = {10000, 10001};
                uint32_t userFriends2[] = {10004, 10005, 10006, 10007};
                uint32_t userFriends4[] = {10008};
                UserGroup group[3] = {
                        {
                                .groupId=0,
                                .groupName="我的好友",
                                .friendCount=2,
                                .friends=userFriends1
                        },{
                                .groupId=0,
                                .groupName="分组",
                                .friendCount=4,
                                .friends=userFriends2
                        },{
                                .groupId=0,
                                .groupName="黑名单",
                                .friendCount=1,
                                .friends=userFriends4
                        },
                };
                UserFriends friends =
                        {
                                .groupCount=3,
                                .groups=group
                        };
                UserSaveFriendsFile(uid, &friends);
            }*/
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, "Status Error");
    }
    return 1;
}