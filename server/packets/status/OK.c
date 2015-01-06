#include <protocol/CRPPackets.h>
#include <logger.h>
#include "run/user.h"

int ProcessPacketStatusOK(POnlineUser user, uint32_t session, CRPPacketOK *packet)
{
    PUserOperation op = UserOperationGet(user, session);
    if (op)
    {
        if (op->onResponseOK(user, op))
            UserOperationDrop(user, op);
    }
    else
    {
        log_info("DEBUG", "Operation Not Found.%u\n", session);
    }
    return 1;
}