#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <asm-generic/errno.h>
#include "run/user.h"
#include "datafile/file.h"

int onCancel(POnlineUser user, PUserOperation operation)
{
    PUserOperationFileStore storeOperation = operation->data;

    if (storeOperation->fd >= 0)
    {
        close(storeOperation->fd);
        unlink(storeOperation->tmpfile);
    }

    UserOperationUnregister(user, operation);
    free(storeOperation);
    return 0;
}

int ProcessPacketFileStoreRequest(POnlineUser user, uint32_t session, CRPPacketFileStoreRequest *packet)
{
    if (user->state == OUS_ONLINE)
    {
        if (packet->type != FST_SHARED)
        {
            CRPFailureSend(user->crp, session, ENOSYS, "不支持的存储类型");
            return 1;
        }
        if (DataFileExist(packet->key))
        {
            CRPFileDataEndSend(user->crp, session, FEC_ALREADY_EXISTS);
        }
        else
        {
            PUserOperationFileStore storeOperation = (PUserOperationFileStore) malloc(sizeof(UserOperationFileStore));
            if (storeOperation == NULL)
            {
                CRPFailureSend(user->crp, session, ENOMEM, "无法创建文件存储操作");
                return 1;
            }
            PUserOperation operation = UserOperationRegister(user, session, CUOT_FILE_STORE, storeOperation);
            if (operation == NULL)
            {
                free(storeOperation);
                CRPFailureSend(user->crp, session, ENOMEM, "无法创建用户操作");
                return 1;
            }
            operation->onCancel = onCancel;
            memcpy(storeOperation->key, packet->key, sizeof(storeOperation->key));
            storeOperation->totalLength = packet->length;
            storeOperation->remainLength = packet->length;
            storeOperation->seq = 0;

            memcpy(storeOperation->tmpfile, "temp/m0MoXXXXXX", sizeof("temp/m0MoXXXXXX"));
            mkstemp(storeOperation->tmpfile);
            storeOperation->fd = creat(storeOperation->tmpfile, 0600);
            if (storeOperation->fd < 0)
            {
                free(storeOperation);
                UserOperationUnregister(user, operation);
                CRPFailureSend(user->crp, session, EIO, "无法创建文件");
                return 1;
            }
            UserOperationDrop(user, operation);
            CRPOKSend(user->crp, session);
        }
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}