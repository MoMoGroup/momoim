#include <protocol/CRPPackets.h>
#include <user.h>

int ProcessPacketLoginLogin(OnlineUser *user, CRPPacketLogin *packet)
{
    CRPOKSend(user->fd);
    return 1;
}