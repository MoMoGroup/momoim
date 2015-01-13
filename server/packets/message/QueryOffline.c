#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <datafile/message.h>
#include <datafile/user.h>
#include <stdlib.h>
#include "run/user.h"

int ProcessPacketMessageQueryOffline(POnlineUser user, uint32_t session, CRPPacketMessageQueryOffline *packet)
{
    if (user->state == OUS_ONLINE)
    {
        MessageFile *file = UserMessageFileGet(user->uid);
        UserInfo *info = UserInfoGet(user->uid);
        if (MessageFileSeek(file, (uint32_t) (info->lastlogout / (24 * 60 * 60))))
        {
            CRPOKSend(user->crp, session);
        }
        else
        {
            CRPFailureSend(user->crp, session, EFAULT, "无法找到下线时间.");
        }
        UserInfoFree(info);
        UserMessage *msg;
        while ((msg = MessageFileNext(file)) != NULL)
        {
            if (msg->time >= info->lastlogout && msg->to == user->uid)
            {
                if (!CRPMessageNormalSend(user->crp, 0, msg->messageType, msg->from, msg->messageLen, msg->content))
                {
                    break;
                }
            }
            free(msg);
        }
        UserMessageFileDrop(user->uid);

    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}