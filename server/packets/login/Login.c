#include <protocol/CRPPackets.h>
#include <user.h>
#include <data/auth.h>

int ProcessPacketLoginLogin(OnlineUser *user, CRPPacketLogin *packet)
{
    if (user->status == OUS_PENDING_LOGIN)
    {
        uint32_t uid;
        int ret = AuthUser(packet->username, packet->password, &uid);
        if (ret != 0)
        {
            CRPFailureSend(user->sockfd, "Login Failure.");
        }
        else
        {
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