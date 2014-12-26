#include <protocol/CRPPackets.h>
#include <user.h>
#include <logger.h>

int ProcessPacketStatusHello(OnlineUser *user, CRPPacketHello *packet)
{
    if (user->status == OUS_PENDING_HELLO)
    {
        if (packet->protocolVersion != 1)
        {
            CRPFailureSend(user->sockfd, "Incompatible protocol version.");
            return 0;
        }
        log_info("UserProc", "Hello.\n");
        CRPOKSend(user->sockfd);
        user->status = OUS_PENDING_LOGIN;
        return 1;
    }
    else
    {
        CRPFailureSend(user->sockfd, "Wrong protocol version.");
        return 0;
    }
}