#include <protocol/CRPPackets.h>
#include <logger.h>
#include <asm-generic/errno-base.h>

#include <run/user.h>
#include "datafile/auth.h"

int ProcessPacketLoginLogin(POnlineUser user, uint32_t session, CRPPacketLogin *packet)
{

    if (user->state == OUS_PENDING_LOGIN)
    {
        uint32_t uid;
        POnlineUser onlineUser = NULL;
        int ret = AuthUser(packet->username, packet->password, &uid);

        if (ret == 0)
        {
            log_info("Login-Login", "User %s Login failure.\n", packet->username);
            CRPFailureSend(user->crp, session, EFAULT, "用户名或密码不正确");
        }
        else
        {
            OnlineUser *duser = OnlineUserGet(uid);
            if (duser)
            {
                CRPKickSend(duser->crp, 0, "另一用户已经登陆");
                OnlineUserDelete(duser);
            }
            onlineUser = UserSetStatus(user, OUS_ONLINE, uid);
            if (!onlineUser)
            {
                log_warning("Login-Login", "User %s Login failure. Cannot Create Online Info\n", packet->username);
                CRPFailureSend(user->crp, session, EFAULT, "服务器内部错误");
                return 0;
            }
            time(&onlineUser->info->loginTime);
            user = onlineUser;
            log_info("Login-Login", "User %s (ID:%u) Login Successful.\n", packet->username, uid);

            CRPLoginAcceptSend(user->crp, session, uid);

        }
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}