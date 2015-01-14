#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketNetDiscoverRefuse(POnlineUser user, uint32_t session, CRPPacketNETDiscoverRefuse *packet)
{

    if (user->state == OUS_ONLINE)
    {
        POnlineUser duser = OnlineUserGet(packet->uid);
        if (!duser)
        {
            CRPFailureSend(user->crp, session, EBADF, "目标已离线");
            return 1;
        }
        CRPNETDiscoverRefuseSend(duser->crp, 0, user->uid);
        UserDrop(duser);
        CRPOKSend(duser->crp, session);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}