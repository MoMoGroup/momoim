#include <protocol/CRPPackets.h>
#include <user.h>

int ProcessPacketStatusKeepAlive(OnlineUser *user, uint32_t session, CRPPacketKeepAlive *packet)
{
    return 1;
}