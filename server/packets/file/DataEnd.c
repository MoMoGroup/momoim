#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <user.h>
#include <unistd.h>
#include <stdio.h>
#include <data/file.h>

static int fn(UserCancelableOperation *op, void *data) {
    return ((UserFileStoreOperation *) op->data)->session == *(uint32_t *) data;
}

int ProcessPacketFileDataEnd(OnlineUser *user, uint32_t session, CRPPacketFileDataEnd *packet)
{
    UserCancelableOperation *op = UserQueryOperation(user, CUOT_FILE_STORE, fn, &session);
    if (!op) {
        CRPFailureSend(user->sockfd, session, "File Store Operation not found.");
    }
    else {
        UserFileStoreOperation *fop = (UserFileStoreOperation *) op->data;
        close(fop->fd);
        fop->fd = -1;
        if (fop->remainLength != 0) {
            unlink(fop->tmpfile);
            CRPFailureSend(user->sockfd, session, "File not finished.");
        }
        else {
            size_t len = DataFilePathLength(fop->key);
            char path[len];
            DataFilePath(fop->key, path);

            if (rename(fop->tmpfile, path)) {
                CRPFailureSend(user->sockfd, session, "File move error.");
            }
            else {
                CRPOKSend(user->sockfd, session);
            }
        }
        UserCancelOperation(user, op->id);
    }
    return 1;
}