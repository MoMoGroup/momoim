#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <unistd.h>
#include <stdio.h>
#include <data/file.h>

static int fn(PUserCancelableOperation op, void *data)
{
    return ((PUserFileStoreOperation) op->data)->session == *(uint32_t *) data;
}

int ProcessPacketFileDataEnd(POnlineUser user, uint32_t session, CRPPacketFileDataEnd *packet)
{
    PUserCancelableOperation op = UserOperationQuery(user, CUOT_FILE_STORE, fn, &session);
    if (!op) {
        CRPFailureSend(user->sockfd, session, "File Store Operation not found.");
    }
    else {
        PUserFileStoreOperation fop = (PUserFileStoreOperation) op->data;
        close(fop->fd);
        fop->fd = -1;
        if (fop->remainLength != 0) {
            unlink(fop->tmpfile);
            CRPFailureSend(user->sockfd, session, "File not finished.");
        }
        else {
            size_t len = DataFilePathLength;
            char path[len];
            DataFilePath(fop->key, path);

            if (rename(fop->tmpfile, path)) {
                CRPFailureSend(user->sockfd, session, "File move error.");
            }
            else {
                CRPOKSend(user->sockfd, session);
            }
        }
        UserOperationCancel(user, op);
    }
    return 1;
}