#include <protocol/CRPPackets.h>
#include "run/user.h"

int ProcessPacketMessageText(OnlineUser *user, uint32_t session, CRPPacketMessageText *packet)
{
    OnlineUser *toUser = OnlineUserGet(packet->uid);
    if (toUser == NULL)
    {

    }
    return 1;
}