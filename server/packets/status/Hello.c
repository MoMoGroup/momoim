#include <protocol/CRPPackets.h>
#include <user.h>
#include <logger.h>

int ProcessPacketStatusHello(OnlineUser *user, uint32_t session, CRPPacketHello *packet)
{
    if (user->status == OUS_PENDING_HELLO)
    {
        if (packet->protocolVersion != 1)
        {
            CRPFailureSend(user->sockfd, session, "Incompatible protocol version.");
            return 0;
        }
        log_info("UserProc", "Hello.\n");
        CRPOKSend(user->sockfd, session);
        user->status = OUS_PENDING_LOGIN;
        return 1;
    }
    else
    {
        CRPFailureSend(user->sockfd, session, "Wrong protocol version.");
        return 0;
    }
}