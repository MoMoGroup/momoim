#include <protocol/CRPPackets.h>
#include <data/user.h>
#include <string.h>
#include <imcommon/friends.h>
#include <protocol/info/Data.h>
#include "run/user.h"

int ProcessPacketInfoData(OnlineUser *user, uint32_t session, CRPPacketInfoData *packet)
{
    if (user->status == OUS_ONLINE)
    {
        if (packet->info.uid != user->info->uid)
        {
            CRPFailureSend(user->sockfd, session, "UID Error");
        }
        else
        {
            if (UserSaveInfoFile(user->info->uid, &packet->info))
                CRPFailureSend(user->sockfd, session, "Write failure");
            else
                CRPOKSend(user->sockfd, session);
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, "Status Error");
    }
    return 1;
}