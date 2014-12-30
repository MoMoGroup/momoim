#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <stdlib.h>
#include <data/user.h>
#include <logger.h>
int ProcessPacketInfoRequest(OnlineUser *user, uint32_t session, CRPPacketInfoRequest *packet)
{
    if (user->status == OUS_ONLINE)
    {
        UserInfo *info = UserGetInfo(packet->uid);
        if (info == NULL)
        {
            CRPFailureSend(user->sockfd, session, "Unable to perform user info.");
        }
        else
        {
            CRPInfoDataSend(user->sockfd, session, info->uid, info->nickName, info->sex, info->icon);
            free(info);
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, "Status Error");
    }
    return 1;
}