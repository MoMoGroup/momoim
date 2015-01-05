#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <unistd.h>
#include <asm-generic/errno-base.h>


int ProcessPacketFileData(POnlineUser user, uint32_t session, CRPPacketFileData *packet)
{
    PUserOperation op = UserOperationGet(user, session);
    if (!op)
    {
        CRPFailureSend(user->sockfd, session, ENOENT, "操作未找到");
    }
    else
    {
        PUserOperationFileStore fop = (PUserOperationFileStore) op->data;
        if (packet->seq != fop->seq)
        {
            CRPFailureSend(user->sockfd, session, EFAULT, "数据序号错误");
        }
        else if (packet->length > fop->remainLength)
        {
            CRPFailureSend(user->sockfd, session, EFBIG, "文件过大");
        }
        else
        {
            write(fop->fd, packet->data, packet->length);
            fop->remainLength -= packet->length;
            fop->seq++;
            CRPOKSend(user->sockfd, session);
        }
    }
    UserOperationDrop(user, op);
    return 1;
}