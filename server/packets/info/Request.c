#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <stdlib.h>
#include <asm-generic/errno.h>
#include "datafile/user.h"

int ProcessPacketInfoRequest(POnlineUser user, uint32_t session, CRPPacketInfoRequest *packet)
{
    if (user->state == OUS_ONLINE)
    {
        UserInfo *info = UserInfoGet(packet->uid);
        if (info == NULL)
        {
            CRPFailureSend(user->crp, session, ENODATA, "无法读取用户资料");
        }
        else
        {
            POnlineUser duser = OnlineUserGet(packet->uid);
            CRPInfoDataSend(user->crp, session, duser != NULL, info);
            if (duser)
            {
                UserDrop(duser);
            }
            UserInfoDrop(info);
        }
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}