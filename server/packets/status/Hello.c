#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <asm-generic/errno.h>

int ProcessPacketStatusHello(POnlineUser user, uint32_t session, CRPPacketHello *packet)
{
    if (user->state == OUS_PENDING_HELLO && packet->protocolVersion == 1)
    {
        UserSetStatus(user, OUS_PENDING_LOGIN, NULL);
        CRPOKSend(user->crp, session);
        return 1;
    }
    else
    {
        CRPFailureSend(user->crp, session, EPROTONOSUPPORT, "不支持的网络协议");
        return 0;
    }
}