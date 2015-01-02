#include <protocol/CRPPackets.h>
#include "run/user.h"

int ProcessPacketFriendSearchByNickname(POnlineUser user, uint32_t session, CRPPacketFriendSearchByNickname *packet)
{
    if (user->status != OUS_ONLINE)
    {
        CRPFailureSend(user->sockfd, session, "Status Error");
    }
    else
    {

    }
    return 1;
}