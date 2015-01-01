#include <protocol/CRPPackets.h>
#include "run/user.h"
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
        OnlineUserSetStatus(user, OUS_PENDING_LOGIN);
        CRPOKSend(user->sockfd, session);
        return 1;
    }
    else
    {
        log_info("UserProc", "%dProtocol Error.\n", user->sockfd);
        CRPFailureSend(user->sockfd, session, "Wrong protocol version.");
        return 0;
    }
}