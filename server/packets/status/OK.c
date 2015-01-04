#include <protocol/CRPPackets.h>
#include "run/user.h"

int ProcessPacketStatusOK(POnlineUser user, uint32_t session, CRPPacketOK *packet)
{
    PUserOperation op = UserOperationGet(user, session);
    if (op)
    {
        op->onResponseOK(user, op);
        UserOperationDrop(op);
    }
    return 1;
}