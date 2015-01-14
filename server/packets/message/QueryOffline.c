#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <datafile/user.h>
#include <stdlib.h>
#include "run/user.h"

int ProcessPacketMessageQueryOffline(POnlineUser user, uint32_t session, CRPPacketMessageQueryOffline *packet)
{
    if (user->state == OUS_ONLINE)
    {
        UserInfo *info = UserInfoGet(user->uid);
        MessageQueryCondition condition = {
                .to=user->uid,
                .time=info->lastlogout,
                .timeOperator=2//>
        };
        UserInfoDrop(info);
        int count;
        UserMessage **p = MessageFileQuery(user->info->message, &condition, &count);
        if (p)
        {
            for (int i = 0; i < count; ++i)
            {
                CRPMessageNormalSend(user->crp,
                                     0,
                                     (USER_MESSAGE_TYPE) (p[i]->messageType),
                                     p[i]->from,
                                     p[i]->messageLen,
                                     p[i]->content);
                free(p[i]);
            }
            CRPOKSend(user->crp, session);
            free(p);
        }
        else
        {
            CRPFailureSend(user->crp, session, ENOENT, "无数据");
        }

    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}