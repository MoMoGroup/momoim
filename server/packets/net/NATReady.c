#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketNETNATReady(POnlineUser user, uint32_t session, CRPPacketNETNATReady *packet)
{

    if (user->state == OUS_ONLINE)
    {
        POnlineUser duser = OnlineUserGet(packet->uid);
        if (duser)
        {
            CRPNETNATReadySend(duser->crp, packet->session, user->uid, session);
            UserDrop(duser);
            CRPOKSend(user->crp, session);
        }
        else
        {
            CRPFailureSend(user->crp, session, ENOENT, "目标已离线");
        }
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}