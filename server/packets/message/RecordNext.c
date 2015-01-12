#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketMessageRecordNext(POnlineUser user, uint32_t session, CRPPacketMessageRecordNext *packet)
{
    if (user->state == OUS_ONLINE)
    {
        uint8_t remain = packet->size;
        while (remain--)
        {
            UserMessage *message = MessageFileNext(user->info->message);
            if (message)
            {
                CRPMessageRecordDataSend(user->crp,
                                         session,
                                         remain,
                                         message);
            }
            else
            {
                break;
            }
        }
        CRPOKSend(user->crp, session);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}