#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <datafile/message.h>
#include <datafile/user.h>
#include "run/user.h"

int ProcessPacketMessageQueryOffline(POnlineUser user, uint32_t session, CRPPacketMessageQueryOffline *packet)
{
    if (user->status == OUS_ONLINE)
    {
        MessageFile *file = UserMessageFileGet(user->info->uid);
        UserInfo *info = UserInfoGet(user->info->uid);

        //PostMessage(msg);
        CRPOKSend(user->sockfd, session);
        //free(msg);
        UserMessageFileDrop(user->info->uid);

    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}