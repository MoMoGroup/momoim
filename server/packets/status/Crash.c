#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <logger.h>

int ProcessPacketStatusCrash(OnlineUser *user, uint32_t session, CRPPacketCrash *packet)
{
    log_error("User", "Client %d Crashed.\n", user->sockfd);
    return 0;
}