#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketNetDiscoverAccept(POnlineUser user, uint32_t session, CRPPacketNETDiscoverAccept *packet)
{

    if (user->state == OUS_ONLINE)
    {
        POnlineUser duser = OnlineUserGet(packet->uid);
        if (!duser)
        {
            CRPFailureSend(user->crp, session, EBADF, "目标已离线");
            return 1;
        }
        struct sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);
        if (getpeername(user->crp->fd, (struct sockaddr *) &addr, &addrLen) == 0)
        {
            CRPNETInetAddressSend(duser->crp, packet->session, user->uid, addr.sin_addr.s_addr);
            UserDrop(duser);
            CRPOKSend(user->crp, session);
        }
        else
        {
            UserDrop(duser);
            CRPFailureSend(user->crp, session, EFAULT, "无法获得网络地址");
        }
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}