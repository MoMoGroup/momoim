#include <protocol/CRPPackets.h>
#include <stdlib.h>
#include <string.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketMessageNormal(POnlineUser user, uint32_t session, CRPPacketMessageNormal *packet)
{
    if (user->state == OUS_ONLINE)
    {
        UserMessage *msg = (UserMessage *) malloc(sizeof(UserMessage) + packet->messageLen);
        msg->messageType = packet->messageType;
        msg->messageLen = packet->messageLen;
        msg->from = user->info->uid;
        msg->to = packet->uid;
        time(&msg->time);
        memcpy(msg->content, packet->message, packet->messageLen);
        PostMessage(msg);
        CRPOKSend(user->crp, session);
        free(msg);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}