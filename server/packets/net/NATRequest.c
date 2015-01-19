#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketNETNATRequest(POnlineUser user, uint32_t session, CRPPacketNETNATRequest *packet)
{
    if (user->state == OUS_ONLINE)
    {
        POnlineUser duser = OnlineUserGet(packet->uid);
        if (duser)
        {
            CRPNETNATRequestSend(duser->crp,
                                 0,
                                 packet->key,
                                 user->uid,
                                 packet->reason,
                                 packet->session);
            UserDrop(duser);
            CRPOKSend(user->crp, session);
        }
        else
        {
            CRPFailureSend(user->crp, session, ENOENT, "目标用户不在线");
        }
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}