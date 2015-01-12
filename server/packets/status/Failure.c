#include <protocol/CRPPackets.h>
#include "run/user.h"

int ProcessPacketStatusFailure(POnlineUser user, uint32_t session, CRPPacketFailure *packet)
{
    PUserOperation op = UserOperationGet(user, session);
    if (op) {
        if (op->onResponseFailure(user, op))
            UserOperationDrop(user, op);
    }
    return 1;
}