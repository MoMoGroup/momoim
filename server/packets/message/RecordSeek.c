#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketMessageRecordSeek(POnlineUser user, uint32_t session, CRPPacketMessageRecordSeek *packet)
{
    if (user->state == OUS_ONLINE)
    {
        if (MessageFileSeek(user->info->message, packet->date))
        {
            CRPOKSend(user->crp, session);
        }
        else
        {
            CRPFailureSend(user->crp, session, EFAULT, "无法重定位");
        }
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}