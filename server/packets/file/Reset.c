#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <unistd.h>
#include <sys/user.h>
#include <asm-generic/errno-base.h>


int ProcessPacketFileReset(POnlineUser user, uint32_t session, CRPPacketFileReset *packet)
{
    PUserOperation op = UserOperationGet(user, session);
    if (!op)
    {
        CRPFailureSend(user->sockfd, session, ENOENT, "操作未找到");
    }
    else
    {
        PUserOperationFileStore fop = (PUserOperationFileStore) op->data;
        off_t ret;
        ret = lseek(fop->fd, packet->seq * PAGE_SIZE, SEEK_SET);
        if (ret)
        {
            CRPFailureSend(fop->fd, session, EINVAL, "无法重定位文件");
        }
        else
        {
            CRPOKSend(fop->fd, session);
        }
        UserOperationDrop(op);
    }
    return 1;
}