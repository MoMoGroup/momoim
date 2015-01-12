#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "datafile/user.h"
#include "run/user.h"

int ProcessPacketInfoData(POnlineUser user, uint32_t session, CRPPacketInfoData *packet)
{
    if (user->state == OUS_ONLINE)
    {
        if (packet->info.uid != user->uid)
        {
            CRPFailureSend(user->crp, session, EINVAL, "无效UID");
        }
        else
        {
            if (!UserInfoSave(user->uid, &packet->info))
            {
                CRPFailureSend(user->crp, session, EFAULT, "无法保存用户资料");
            }
            else
            {
                CRPOKSend(user->crp, session);
            }
        }
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}