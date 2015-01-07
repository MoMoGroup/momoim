#include <protocol/CRPPackets.h>
#include <logger.h>
#include <asm-generic/errno-base.h>
#include <unistd.h>

#include <run/user.h>
#include "datafile/auth.h"
#include "datafile/friend.h"

int ProcessPacketLoginLogin(POnlineUser user, uint32_t session, CRPPacketLogin *packet)
{
#ifndef NDEBUG
    sleep(1);//For Debugger
#endif
    if (user->status == OUS_PENDING_LOGIN)
    {
        uint32_t uid;
        POnlineUser onlineUser = NULL;
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
            onlineUser = UserSwitchToOnline((PPendingUser) user, uid);
            if (!onlineUser)
            {
                log_warning("Login-Login", "User %s Login failure. Cannot Create Online Info\n", packet->username);
                CRPFailureSend(user->sockfd, session, EFAULT, "服务器内部错误");
                return 0;
            }
            user = onlineUser;
            log_info("Login-Login", "User %s (ID:%u) Login Successful.\n", packet->username, uid);

            CRPLoginAcceptSend(user->sockfd, session, uid);

        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}