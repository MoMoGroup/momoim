#include <protocol/CRPPackets.h>
#include "run/user.h"

int ProcessPacketStatusOK(OnlineUser *user, uint32_t session, CRPPacketOK *packet)
{
    return 1;
}