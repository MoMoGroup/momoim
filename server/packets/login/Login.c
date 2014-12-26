#include <protocol/CRPPackets.h>
#include <user.h>
#include <data/auth.h>
#include <logger.h>
#include <unistd.h>

int ProcessPacketLoginLogin(OnlineUser *user, CRPPacketLogin *packet)
{
    sleep(2);
    if (user->status == OUS_PENDING_LOGIN)
    {
        uint32_t uid;
        int ret = AuthUser(packet->username, packet->password, &uid);
        if (ret != 0)
        {
            log_info("Login-Login", "User %s Login failure.", packet->username);
            CRPFailureSend(user->sockfd, "Login Failure.");
        }
        else
        {
            log_info("Login-Login", "User %s (ID:%u) Login Successful.", packet->username, uid);
            CRPLoginAcceptSend(user->sockfd, uid);
            user->status = OUS_ONLINE;
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, "Status Error");
    }
    return 1;
}