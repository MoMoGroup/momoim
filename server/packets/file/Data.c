#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <user.h>
#include <unistd.h>

static int fn(UserCancelableOperation *op, void *data)
{
    return ((UserFileStoreOperation *) op->data)->session == *(uint32_t *) data;
}

int ProcessPacketFileData(OnlineUser *user, uint32_t session, CRPPacketFileData *packet) {
    UserCancelableOperation *op = UserQueryOperation(user, CUOT_FILE_STORE, fn, &session);
    if (!op) {
        CRPFailureSend(user->sockfd, session, "File Store Operation not found.");
    }
    else {
        UserFileStoreOperation *fop = (UserFileStoreOperation *) op->data;
        pthread_mutex_lock(&fop->lock);
        if (packet->seq != fop->seq) {
            CRPFailureSend(user->sockfd, session, "Part sequence wrong.");
        }
        else if (packet->length > fop->remainLength) {
            CRPFailureSend(user->sockfd, session, "File too large.");
        }
        else {
            write(fop->fd, packet->data, packet->length);
            fop->remainLength -= packet->length;
            fop->seq++;
            CRPOKSend(user->sockfd, session);
        }
        pthread_mutex_unlock(&fop->lock);
    }
    return 1;
}