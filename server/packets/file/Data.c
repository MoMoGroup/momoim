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
        CRPFailureSend(user->crp, session, ENOENT, "操作未找到");
    }
    else {
        PUserOperationFileStore fop = (PUserOperationFileStore) op->data;
        if (packet->seq != fop->seq)//数据包序号与期待的序号不一致.要求客户端重置
        {
            CRPFileResetSend(user->crp, session, fop->seq);
        }
        else if (packet->length > fop->remainLength)
        {
            CRPFailureSend(user->crp, session, EFBIG, "文件长度与预期不一致");
        }
        else {
            write(fop->fd, packet->data, packet->length);
            fop->remainLength -= packet->length;
            ++fop->seq;
            CRPOKSend(user->crp, session);
        }
        UserOperationDrop(user, op);
    }
    return 1;
}