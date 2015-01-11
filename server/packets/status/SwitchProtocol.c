#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <openssl/md5.h>
#include "run/user.h"

int ProcessPacketStatusSwitchProtocol(POnlineUser user, uint32_t session, CRPPacketSwitchProtocol *packet)
{
    if (user->state == OUS_PENDING_LOGIN || user->state == OUS_ONLINE)
    {
        unsigned char buf[32], sendKey[32];
        clock_gettime(CLOCK_REALTIME_COARSE, (struct timespec *) buf);
        clock_gettime(CLOCK_MONOTONIC_COARSE, (struct timespec *) (buf + 16));
        MD5(buf, 32, sendKey);

        CRPSwitchProtocolSend(user->crp, session, (char *) sendKey, packet->iv);
        CRPEncryptEnable(user->crp, (char *) sendKey, packet->key, packet->iv);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}