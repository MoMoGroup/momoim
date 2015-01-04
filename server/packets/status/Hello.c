#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <asm-generic/errno.h>

int ProcessPacketStatusHello(POnlineUser user, uint32_t session, CRPPacketHello *packet)
{
    if (user->status == OUS_PENDING_HELLO && packet->protocolVersion == 1)
    {
        OnlineUserSetStatus(user, OUS_PENDING_LOGIN, NULL);
        CRPOKSend(user->sockfd, session);
        return 1;
    }
    else
    {
        CRPFailureSend(user->sockfd, session, EPROTONOSUPPORT, "不支持的网络协议");
        return 0;
    }
}