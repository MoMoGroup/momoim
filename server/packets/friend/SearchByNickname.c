#include <protocol/CRPPackets.h>
#include "run/user.h"

int ProcessPacketFriendSearchByNickname(OnlineUser *user, uint32_t session, CRPPacketFriendSearchByNickname *packet)
{
    if (user->status != OUS_ONLINE)
    {

    }
    return 1;
}