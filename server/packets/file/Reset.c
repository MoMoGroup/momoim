#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <unistd.h>
#include <sys/user.h>
#include <asm-generic/errno.h>


int ProcessPacketFileReset(POnlineUser user, uint32_t session, CRPPacketFileReset *packet)
{
    PUserOperation op = UserOperationGet(user, session);
    if (!op)
    {
        CRPFailureSend(user->sockfd, session, ENOENT, "操作未找到");
    }
    else
    {
        switch (op->type)
        {
            case CUOT_FILE_REQUEST:
            {
                PUserOperationFileRequest opData = (PUserOperationFileRequest) op->data;
                aio_cancel(opData->aio.aio_fildes, &opData->aio);
                opData->aio.aio_offset = packet->seq * opData->aio.aio_nbytes;
                if (-1 == aio_read(&opData->aio))
                {
                    CRPFailureSend(user->sockfd, session, EFAULT, "重定位失败");
                }
                else
                {
                    CRPOKSend(user->sockfd, session);
                }
                break;
            }
            case CUOT_FILE_STORE:
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
                break;
            }
            default:
                CRPFailureSend(user->sockfd, session, ENOSYS, "该操作类型不支持重置");
        }
        UserOperationDrop(op);
    }
    return 1;
}