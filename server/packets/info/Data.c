#include <protocol/CRPPackets.h>
#include <data/user.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketInfoData(POnlineUser user, uint32_t session, CRPPacketInfoData *packet)
{
    if (user->status == OUS_ONLINE)
    {
        if (packet->info.uid != user->info->uid)
        {
            CRPFailureSend(user->sockfd, session, EINVAL, "无效UID");
        }
        else
        {
            if (!UserInfoSave(user->info->uid, &packet->info))
                CRPFailureSend(user->sockfd, session, EFAULT, "无法保存用户资料");
            else
                CRPOKSend(user->sockfd, session);
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}