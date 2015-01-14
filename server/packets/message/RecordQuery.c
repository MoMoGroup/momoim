#include <protocol/CRPPackets.h>
#include <run/user.h>
#include <asm-generic/errno-base.h>
#include <stdlib.h>

int ProcessPacketMessageRecordQuery(POnlineUser user, uint32_t session, CRPPacketMessageRecordQuery *packet)
{
    if (user->state == OUS_ONLINE)
    {
        int count;
        UserMessage **p = MessageFileQuery(user->info->message, &packet->condition, &count);
        if (p)
        {
            for (int i = 0; i < count; ++i)
            {
                CRPMessageRecordDataSend(user->crp, session, (uint8_t) (count - i), p[i]);
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