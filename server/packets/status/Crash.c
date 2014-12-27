#include <protocol/CRPPackets.h>
#include <user.h>
#include <logger.h>

int ProcessPacketStatusCrash(OnlineUser *user, CRPPacketCrash *packet)
{
    log_error("User", "Client %d Crashed.\n", user->sockfd);
    return 0;
}