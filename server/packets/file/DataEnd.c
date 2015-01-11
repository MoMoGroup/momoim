#include <unistd.h>
#include <stdio.h>
#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include "protocol.h"
#include "run/user.h"
#include "datafile/file.h"

int ProcessPacketFileDataEnd(POnlineUser user, uint32_t session, CRPPacketFileDataEnd *packet)
{
    PUserOperation op = UserOperationGet(user, session);
    if (!op)
    {
        CRPFailureSend(user->crp, session, ENOENT, "操作未找到");
    }
    else
    {
        PUserOperationFileStore fop = (PUserOperationFileStore) op->data;
        close(fop->fd);
        fop->fd = -1;
        if (fop->remainLength != 0)
        {
            unlink(fop->tmpfile);
            CRPFailureSend(user->crp, session, EBADF, "文件未结束");
        }
        else
        {
            size_t len = DataFilePathLength;
            char *path = (char *) malloc(len);
            DataFilePath(fop->key, path);

            if (rename(fop->tmpfile, path) != 0)//FEATURE 文件移动失败,可能目标不在同一文件系统.需要进行复制.
            {
                unlink(fop->tmpfile);
                CRPFailureSend(user->crp, session, EFAULT, "文件移动失败\n");
            }
            else
            {
                CRPOKSend(user->crp, session);
            }
            free(path);
        }
        UserOperationUnregister(user, op);
    }
    return 1;
}