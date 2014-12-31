#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include "run/user.h"
#include <unistd.h>
#include <sys/user.h>

static int fn(UserCancelableOperation *op, void *data)
{
    return ((UserFileStoreOperation *) op->data)->session == *(uint32_t *) data;
}

int ProcessPacketFileReset(OnlineUser *user, uint32_t session, CRPPacketFileReset *packet)
{
    UserCancelableOperation *op = UserOperationQuery(user, CUOT_FILE_STORE, fn, &session);
    if (!op)
    {
        CRPFailureSend(user->sockfd, session, "File Store Operation not found.");
    }
    else
    {
        UserFileStoreOperation *fop = (UserFileStoreOperation *) op->data;
        off_t ret;
        pthread_mutex_lock(&fop->lock);
        ret = lseek(fop->fd, packet->seq * PAGE_SIZE, SEEK_SET);
        pthread_mutex_unlock(&fop->lock);
        if (ret)
        {
            CRPFailureSend(fop->fd, session, "Unable to reset file offset");
        }
        else
        {
            CRPOKSend(fop->fd, session);
        }
    }
    return 1;
}