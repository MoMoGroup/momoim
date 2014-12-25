#include <protocol/CRPPackets.h>
#include <user.h>
#include <logger.h>

int ProcessPacketStatusHello(OnlineUser *user, CRPPacketHello *packet)
{
    if (user->status == OUS_PENDING_HELLO)
    {
        if (packet->protocolVersion != 1)
        {
            //log_info("UserProc", "Try to use wrong protocol to login.\n");
            CRPStatusFailureSend(user->fd, "Wrong protocol version.");
            return 0;
        }
        log_info("UserProc", "Hello.\n");
        CRPOKSend(user->fd);
        user->status = OUS_PENDING_LOGIN;
    }
    else
    {
        CRPStatusFailureSend(user->fd, "Wrong protocol version.");
        return 0;
    }
}