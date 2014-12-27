#include <protocol/CRPPackets.h>
#include <user.h>
#include <data/auth.h>
#include <logger.h>
#include <unistd.h>

int ProcessPacketLoginLogin(OnlineUser *user, CRPPacketLogin *packet)
{
    sleep(1);//For Debugger Only
    if (user->status == OUS_PENDING_LOGIN)
    {
        uint32_t uid;
        int ret = AuthUser(packet->username, packet->password, &uid);

        if (ret != 0)
        {
            log_info("Login-Login", "User %s Login failure.\n", packet->username);
            CRPFailureSend(user->sockfd, "Login Failure.");
        }
        else
        {
            user->info = UserCreateOnlineInfo(user, uid);
            if (user->info == NULL)
            {
                log_warning("Login-Login", "User %s Login failure. Cannot Create Online Info\n", packet->username);
                CRPFailureSend(user->sockfd, "Server Failure.");
            }
            else
            {
                log_info("Login-Login", "User %s (ID:%u) Login Successful.\n", packet->username, uid);
                CRPLoginAcceptSend(user->sockfd, uid);
                user->status = OUS_ONLINE;
            }
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, "Status Error");
    }
    return 1;
}