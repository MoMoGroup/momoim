#include <protocol/CRPPackets.h>
#include <user.h>
#include <stdlib.h>
#include <data/user.h>

int ProcessPacketInfoRequest(OnlineUser *user, uint32_t session, CRPPacketInfoRequest *packet)
{
    if (user->status == OUS_ONLINE)
    {
        UserInfo *info = UserGetInfo(user->info->uid);
        if (info == NULL)
        {
            CRPFailureSend(user->sockfd, session, "Unable to perform user info.");
        }
        else
        {
            CRPInfoDataSend(user->sockfd, session, info->uid, info->nickName, info->sex);
            free(info);
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, "Status Error");
    }
    return 1;
}