#include <protocol/CRPPackets.h>
#include <user.h>
#include <stdlib.h>

int ProcessPacketInfoQuery(OnlineUser *user, CRPPacketInfoQuery *packet)
{
    if (user->status == OUS_ONLINE)
    {
        UserInfo *info = UserGetInfo(user->info->uid);
        if (info == NULL)
        {
            CRPFailureSend(user->sockfd, "Unable to perform user info.");
        }
        else
        {
            CRPInfoDataSend(user->sockfd, info->uid, info->nickName, info->sex);
            free(info);
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, "Status Error");
    }
    return 1;
}