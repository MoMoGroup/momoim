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
        CRPFailureSend(user->sockfd, session, ENOENT, "操作未找到");
    }
    else
    {
        PUserOperationFileStore fop = (PUserOperationFileStore) op->data;
        close(fop->fd);
        fop->fd = -1;
        if (fop->remainLength != 0)
        {
            unlink(fop->tmpfile);
            CRPFailureSend(user->sockfd, session, EBADF, "文件未结束");
        }
        else
        {
            size_t len = DataFilePathLength;
            char *path = (char *) malloc(len);
            DataFilePath(fop->key, path);

            if (rename(fop->tmpfile, path))
            {
                perror("rename");
                CRPFailureSend(user->sockfd, session, EFAULT, "文件移动失败\n");
            }
            else
            {
                CRPOKSend(user->sockfd, session);
            }
            free(path);
        }
        UserOperationUnregister(user, op);
    }
    return 1;
}