#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <data/file.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int onCancel(struct struOnlineUser *user, struct struUserCancelableOperation *operation)
{
    UserFileStoreOperation *storeOperation = operation->data;
    pthread_mutex_lock(&storeOperation->lock);

    if (storeOperation->fd >= 0)
    {
        close(storeOperation->fd);
        unlink(storeOperation->tmpfile);
    }

    pthread_mutex_unlock(&storeOperation->lock);
    pthread_mutex_destroy(&storeOperation->lock);
    UserOperationUnregister(user, operation);
    free(storeOperation);
    return 1;
}

int ProcessPacketFileStoreRequest(OnlineUser *user, uint32_t session, CRPPacketFileStoreRequest *packet)
{
    if (user->status == OUS_ONLINE)
    {
        if (packet->type == FST_PRIVATE)
        {
            CRPFailureSend(user->sockfd, session, "Private file is not support.");
            return 1;
        }
        if (DataFileExist(packet->key))
        {
            CRPFileDataEndSend(user->sockfd, session, FEC_ALREADY_EXISTS);
        }
        else
        {
            UserCancelableOperation *operation = UserOperationRegister(user, CUOT_FILE_STORE);
            if (operation == NULL)
            {
                CRPFailureSend(user->sockfd, session, "Fail to register operation.");
                return 1;
            }
            UserFileStoreOperation *storeOperation = (UserFileStoreOperation *) malloc(sizeof(UserFileStoreOperation));
            if (storeOperation == NULL)
            {
                UserOperationUnregister(user, operation);
                CRPFailureSend(user->sockfd, session, "Fail to alloc store operation.");
                return 1;
            }
            operation->data = storeOperation;
            operation->onCancel = onCancel;
            memcpy(storeOperation->key, packet->key, sizeof(storeOperation->key));
            storeOperation->totalLength = packet->length;
            storeOperation->remainLength = packet->length;

            memcpy(storeOperation->tmpfile, "/tmp/m0MoXXXXXX", sizeof("/tmp/m0MoXXXXXX"));
            mkstemp(storeOperation->tmpfile);
            storeOperation->fd = creat(storeOperation->tmpfile, 0600);
            if (storeOperation->fd < 0) {
                free(storeOperation);
                UserOperationUnregister(user, operation);
                CRPFailureSend(user->sockfd, session, "Fail to create file.");
                return 1;
            }
            storeOperation->session = session;
            pthread_mutex_init(&storeOperation->lock, NULL);
            CRPFileStoreAcceptSend(user->sockfd, session, operation->id);
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, "Status Error");
    }
    return 1;
}