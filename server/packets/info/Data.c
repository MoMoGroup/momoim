#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <protocol/info/Data.h>
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
            UserInfo *info = UserInfoGet(user->uid);
            packet->info.lastlogout = info->lastlogout;
            packet->info.level = info->level;
            UserInfoDrop(info);
            if (!UserInfoSave(user->uid, &packet->info))
            {
                CRPFailureSend(user->crp, session, EFAULT, "无法保存用户资料");
            }
            else
            {
                UserBroadcastNotify(user, FNT_FRIEND_INFO_CHANGED);
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