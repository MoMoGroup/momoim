#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <openssl/md5.h>
#include "run/user.h"

int ProcessPacketStatusSwitchProtocol(POnlineUser user, uint32_t session, CRPPacketSwitchProtocol *packet)
{
    if (user->status == OUS_PENDING_LOGIN || user->status == OUS_ONLINE)
    {
        unsigned char buf[32], sendKey[32];
        clock_gettime(CLOCK_REALTIME_COARSE, (struct timespec *) buf);
        clock_gettime(CLOCK_MONOTONIC_COARSE, (struct timespec *) (buf + 16));
        MD5(buf, 32, sendKey);

        CRPSwitchProtocolSend(user->sockfd, session, (char *) sendKey, packet->iv);
        CRPEncryptEnable(user->sockfd, (char *) sendKey, packet->key, packet->iv);
    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}