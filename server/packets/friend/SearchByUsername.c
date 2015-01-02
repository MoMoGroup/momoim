#include <protocol/CRPPackets.h>
#include <run/user.h>


int ProcessPacketFriendSearchByUsername(POnlineUser user, uint32_t session, CRPPacketFriendSearchByUsername *packet)
{
    if (user->status != OUS_ONLINE)
    {

    }
    return 1;
}