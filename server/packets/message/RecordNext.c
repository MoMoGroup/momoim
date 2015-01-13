#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include "run/user.h"

int ProcessPacketMessageRecordNext(POnlineUser user, uint32_t session, CRPPacketMessageRecordNext *packet)
{
    if (user->state == OUS_ONLINE)
    {
        uint8_t remain = packet->size;
        while (remain)
        {
            UserMessage *message = MessageFileNext(user->info->message);
            if (message)
            {
                if (packet->uid == 0 || message->from == packet->uid || message->to == packet->uid)
                {
                    --remain;
                    if (!CRPMessageRecordDataSend(user->crp, session, remain, message))
                    {
                        return 0;
                    }
                }
                free(message);
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