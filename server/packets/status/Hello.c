#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <asm-generic/errno.h>
#include <server.h>

int ProcessPacketStatusHello(POnlineUser user, uint32_t session, CRPPacketHello *packet)
{
    if (user->state == OUS_PENDING_HELLO && packet->protocolVersion == 1)
    {
        UserSetState(user, OUS_PENDING_LOGIN, 0);
        EpollAdd(user);//非已登陆用户
        CRPOKSend(user->crp, session);
        return 1;
    }
    else
    {
        CRPFailureSend(user->crp, session, EPROTONOSUPPORT, "不支持的网络协议");
        return 0;
    }
}