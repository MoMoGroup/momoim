#include <unistd.h>
#include <stdlib.h>
#include <sys/user.h>
#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <errno.h>
#include "run/user.h"
#include "data/file.h"

static int onRequestCancel(POnlineUser user, PUserOperation op)
{
    if (op->type == CUOT_FILE_REQUEST)
    {
        PUserOperationFileRequest opData = (PUserOperationFileRequest) op->data;
        aio_cancel(opData->aio.aio_fildes, NULL);//Cancel all
        close(opData->aio.aio_fildes);
        free((void *) opData->aio.aio_buf);
        free(opData);
        op->onCancel = NULL;
        UserOperationUnregister(user, op);
        CRPFileDataEndSend(user->sockfd, op->session, FEC_CANCELED);
        return 0;
    }
    return 1;
}

static int RequestContinue(POnlineUser user, PUserOperation op)
{
    if (op->type == CUOT_FILE_REQUEST)
    {
        PUserOperationFileRequest opData = (PUserOperationFileRequest) op->data;
        const struct aiocb *const p = &opData->aio;
        struct timespec timeout = {
                .tv_sec=1
        };
        int ret;
        while ((ret = aio_suspend(&p, 1, &timeout) == -1) && errno == EINTR);
        if (ret == -1)
        {
            CRPFailureSend(user->sockfd, op->session, ETIMEDOUT, "操作超时");
        }
        switch (aio_error(p))
        {
            case EINPROGRESS:
                CRPFailureSend(user->sockfd, op->session, ETIMEDOUT, "操作超时");
                break;
            case ECANCELED:
                CRPFailureSend(user->sockfd, op->session, ECANCELED, "操作被取消");
                break;
            case 0:

                break;
            default:
                CRPFailureSend(user->sockfd, op->session, EIO, "操作失败");
                break;
        }
        ssize_t ioRet = aio_return(&opData->aio);
        if (ioRet == 0)
        {
            CRPFileDataEndSend(user->sockfd, op->session, FEC_OK);
            close(opData->aio.aio_fildes);
            free((void *) opData->aio.aio_buf);
            free(opData);
            op->onCancel = NULL;
            op->cancel = 1;
            UserOperationUnregister(user, op);
            return 0;
        }
        else
        {
            CRPFileDataSend(user->sockfd, op->session, (CRP_LENGTH_TYPE) ioRet, opData->seq++, (void *) p->aio_buf);
            opData->aio.aio_offset += opData->aio.aio_nbytes;
            aio_read(&opData->aio);
        }
    }
    return 1;
}

int ProcessPacketFileRequest(POnlineUser user, uint32_t session, CRPPacketFileRequest *packet)
{
    if (packet->type == FST_SHARED)
    {
        if (DataFileExist(packet->key))
        {
            PUserOperation op = NULL;
            PUserOperationFileRequest opData = NULL;
            int fd = DataFileOpen(packet->key, O_RDONLY);
            if (fd < 0)
            {
                CRPFailureSend(user->sockfd, session, ENOENT, "无法读取文件");
                goto fail;
            }
            struct stat fileInfo;
            if (fstat(fd, &fileInfo) == -1)
            {
                CRPFailureSend(user->sockfd, session, EIO, "无法获得文件状态");
                goto fail;
            }
            opData = (PUserOperationFileRequest) calloc(1, sizeof(UserOperationFileRequest));
            if (opData == NULL)
            {
                CRPFailureSend(user->sockfd, session, ENOMEM, "无法创建文件请求操作");
                goto fail;
            }
            opData->seq = 0;
            opData->aio.aio_buf = (char *) malloc(PAGE_SIZE);
            if (opData->aio.aio_buf == NULL)
            {
                CRPFailureSend(user->sockfd, session, ENOMEM, "无法分配缓冲区");
                goto fail;
            }
            opData->aio.aio_fildes = fd;
            opData->aio.aio_nbytes = PAGE_SIZE;
            opData->size = fileInfo.st_size;
            op = UserOperationRegister(user, session, CUOT_FILE_REQUEST, opData);
            if (op == NULL)
            {
                CRPFailureSend(user->sockfd, session, EMFILE, "无法创建用户操作");
                goto fail;
            }
            op->onCancel = onRequestCancel;
            op->onResponseOK = RequestContinue;
            op->onResponseFailure = onRequestCancel;
            aio_read(&opData->aio);
            UserOperationDrop(user, op);
            CRPFileDataStartSend(user->sockfd, session, (uint64_t) fileInfo.st_size);
            return 1;
            fail:
            if (op)
            {
                UserOperationUnregister(user, op);
            }
            if (fd >= 0)
                close(fd);
            if (opData)
            {
                free((void *) opData->aio.aio_buf);
                free(opData);
            }
            return 1;
        }
        else
        {
            CRPFailureSend(user->sockfd, session, ENOENT, "文件未找到");
        }
    }
    else
    {
        if (user->status != OUS_ONLINE)
        {
            CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
        }
        else
        {
            CRPFailureSend(user->sockfd, session, ENOSYS, "不支持的文件类型");
        }
    }
    return 1;
}