#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketInfoStatusChange(POnlineUser user, uint32_t session, CRPPacketInfoStatusChange *packet)
{
    if (user->state == OUS_ONLINE)
    {
        user->hiddenStatus = packet->hidden ? UOS_HIDDEN : UOS_ONLINE;
        if (packet->hidden)
        {
            UserBroadcastNotify(user, FNT_FRIEND_OFFLINE);
        }
        else
        {
            UserBroadcastNotify(user, FNT_FRIEND_ONLINE);
        }
        CRPOKSend(user->crp, session);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}