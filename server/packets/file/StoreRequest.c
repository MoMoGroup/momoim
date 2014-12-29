#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <user.h>
#include <data/file.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int onCancel(struct struOnlineUser *user, struct struUserCancelableOperation *operation)
{
    UserFileStoreOperation *storeOperation = operation->data;
    close(storeOperation->fd);
    free(storeOperation->tmpfile);
    free(storeOperation);
    UserUnregisterOperation(user, operation);
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
            char file[] = "m0MoXXXXXX";
            if (mkstemp(file) == -1)
            {
                CRPFailureSend(user->sockfd, session, "Fail to create file.");
                return 1;
            }
            int fd = creat(file, 0600);
            if (fd < 0)
            {
                CRPFailureSend(user->sockfd, session, "Fail to create file.");
                return 1;
            }
            UserCancelableOperation *operation = UserRegisterOperation(user);
            if (operation == NULL)
            {
                CRPFailureSend(user->sockfd, session, "Too many operation.");
                close(fd);
                return 1;
            }
            UserFileStoreOperation *storeOperation = (UserFileStoreOperation *) malloc(sizeof(UserFileStoreOperation));
            operation->data = storeOperation;
            operation->oncancel = onCancel;
            memcpy(storeOperation->key, packet->key, sizeof(storeOperation->key));
            storeOperation->totalLength = packet->length;
            storeOperation->remainLength = packet->length;
            storeOperation->fd = fd;
            storeOperation->tmpfile = (char *) malloc(sizeof(file));
            memcpy(storeOperation->tmpfile, file, sizeof(file));

            CRPFileStoreAcceptSend(user->sockfd, session, operation->id);
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, "Status Error");
    }
    return 1;
}