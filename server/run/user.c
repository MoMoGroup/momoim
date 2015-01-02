#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <logger.h>
#include <packets.h>
#include <unistd.h>
#include <sys/stat.h>
#include "data/user.h"
#include <server.h>
#include <run/jobs.h>

OnlineUsersTableType OnlineUserTable = {
        .count=0,
        .first=NULL,
        .last=NULL
};

int(*PacketsProcessMap[CRP_PACKET_ID_MAX + 1])(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header) = {
        [CRP_PACKET_KEEP_ALIVE]         = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusKeepAlive,
        [CRP_PACKET_HELLO]              = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusHello,
        [CRP_PACKET_OK]                 = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusOK,
        [CRP_PACKET_FAILURE]            = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusFailure,
        [CRP_PACKET_CRASH]              = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusCrash,

        [CRP_PACKET_LOGIN__START]       = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_LOGIN_LOGIN]        = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketLoginLogin,
        [CRP_PACKET_LOGIN_LOGOUT]       = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketLoginLogout,
        [CRP_PACKET_LOGIN_REGISTER]     = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketLoginRegister,

        [CRP_PACKET_INFO__START]        = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_INFO_REQUEST]       = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketInfoRequest,
        [CRP_PACKET_INFO_DATA]          = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketInfoData,

        [CRP_PACKET_FRIEND__START]      = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_FRIEND_REQUEST]     = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFriendRequest,
        [CRP_PACKET_FRIEND_DATA]        = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,

        [CRP_PACKET_FILE__START]        = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_FILE_REQUEST]       = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileRequest,
        [CRP_PACKET_FILE_DATA]          = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileData,
        [CRP_PACKET_FILE_DATA_END]      = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileDataEnd,
        [CRP_PACKET_FILE_STORE_REQUEST] = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileStoreRequest,


        [CRP_PACKET_MESSAGE__START]     = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_MESSAGE_NORMAL]       = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketMessageText,
};

/**
* Server Process User Message
*/
int ProcessUser(POnlineUser user, CRPBaseHeader *packet)
{
    int ret = 1;
    void *data;
    void *(*packetCast)(CRPBaseHeader *) = PacketsDataCastMap[packet->packetID];
    if (packetCast == NULL)
    {
        return 0;
    }
    data = packetCast(packet);
    if (data == NULL)
    {
        return 0;
    }

    int(*packetProcessor)(POnlineUser, uint32_t, void *, CRPBaseHeader *header) = PacketsProcessMap[packet->packetID];
    if (packetProcessor != NULL)
    {
        ret = packetProcessor(user, packet->sessionID, data, packet);
        if (data != packet->data)
        {
            free(data);
        }
    }
    else
    {
        log_warning("UserProc", "Packet %d has no handler.\n", packet->packetID);
    }
    return ret;
}

POnlineUser OnlineUserNew(int fd)
{
    POnlineUser user = (POnlineUser) calloc(1, sizeof(OnlineUser));
    if (user == NULL)
    {
        log_error("UserManager", "Fail to calloc new user.\n");
        return NULL;
    }
    user->sockfd = fd;
    user->status = OUS_PENDING_INIT;
    pthread_rwlock_init(&user->holdLock, NULL);

    return user;
}

void OnlineUserInit(POnlineUser user)
{
    if (!user->status == OUS_PENDING_INIT)
        return;
    pthread_rwlock_init(&user->operations.lock, NULL);

    user->status = OUS_PENDING_HELLO;

    pthread_rwlock_wrlock(&OnlineUserTable.lock);

    if (OnlineUserTable.last == NULL)
    {
        OnlineUserTable.first = OnlineUserTable.last = user;
    }
    else
    {
        OnlineUserTable.last->next = user;
        user->prev = OnlineUserTable.last;
        OnlineUserTable.last = user;
    }
    ++OnlineUserTable.count;
    pthread_rwlock_unlock(&OnlineUserTable.lock);

}

int OnlineUserDelete(POnlineUser user)
{
    pthread_rwlock_unlock(&user->holdLock);
    pthread_rwlock_wrlock(&user->holdLock);
    if (user->status == OUS_PENDING_INIT)
    {
        pthread_rwlock_unlock(&user->holdLock);
        pthread_rwlock_destroy(&user->holdLock);
        free(user);
        return 0;
    }
    else if (user->status == OUS_PENDING_CLEAN)
    {
        return 0;
    }
    user->status = OUS_PENDING_CLEAN;
    JobManagerKick(user);
    UserRemoveFromPool(user);
    UserOperationRemoveAll(user);

    shutdown(user->sockfd, SHUT_RDWR);
    close(user->sockfd);
    UserFreeOnlineInfo(user);

    pthread_rwlock_destroy(&user->operations.lock);

    pthread_rwlock_wrlock(&OnlineUserTable.lock);
    if (user->prev != NULL || user->next != NULL || OnlineUserTable.first == user)
    {
        if (user->prev == NULL)
        {
            OnlineUserTable.first = user->next;
        }
        else
        {
            user->prev->next = user->next;
        }
        if (user->next == NULL)
        {
            OnlineUserTable.last = user->prev;
        }
        else
        {
            user->next->prev = user->prev;
        }
        --OnlineUserTable.count;
    }
    pthread_rwlock_unlock(&OnlineUserTable.lock);

    pthread_rwlock_unlock(&user->holdLock);
    pthread_rwlock_destroy(&user->holdLock);

    free(user);
    return 1;
}

int OnlineUserHold(POnlineUser user)
{
    return user->status != OUS_PENDING_CLEAN && pthread_rwlock_tryrdlock(&user->holdLock) == 0;
}

void OnlineUserDrop(POnlineUser user)
{
    pthread_rwlock_unlock(&user->holdLock);
}

POnlineUser OnlineUserGet(uint32_t uid)
{
    POnlineUser ret = NULL;
    pthread_rwlock_rdlock(&OnlineUserTable.lock);
    for (POnlineUser user = OnlineUserTable.first; user != NULL; user = user->next)
    {
        if (user->status == OUS_ONLINE && user->info->uid == uid && OnlineUserHold(user))
        {
            ret = user;
            break;
        }
    }
    pthread_rwlock_unlock(&OnlineUserTable.lock);
    return ret;
}

void OnlineUserSetStatus(POnlineUser user, OnlineUserStatus status)
{
    user->status = status;
}

POnlineUserInfo UserCreateOnlineInfo(POnlineUser user, uint32_t uid)
{
    char userDir[30];
    uint8_t userDirSize;
    UserGetDir(userDir, uid, "");
    userDirSize = (uint8_t) strlen(userDir);
    struct stat buf;
    if (stat(userDir, &buf) || !S_ISDIR(buf.st_mode))
    {
        UserCreateDirectory(uid);
    }
    POnlineUserInfo info = (POnlineUserInfo) malloc(sizeof(OnlineUser));
    memset(info, 0, sizeof(OnlineUserInfo));
    info->uid = uid;
    info->userDir = (char *) malloc(userDirSize + 1);
    memcpy(info->userDir, userDir, userDirSize);
    info->userDir[userDirSize] = 0;
    return info;
}

void UserFreeOnlineInfo(POnlineUser user)
{
    if (user->info)
    {
        if (user->info->userDir)
        {
            free(user->info->userDir);
        }
        free(user->info);
    }
}

PUserCancelableOperation UserOperationRegister(POnlineUser user, int type)
{
/*  the can not support this feature
    if (user->operations.count >= 100)
        return NULL;
*/
    PUserCancelableOperation operation = (PUserCancelableOperation) calloc(1, sizeof(UserCancelableOperation));
    if (operation == NULL)
    {
        return NULL;
    }
    operation->next = NULL;
    operation->type = type;
    pthread_rwlock_wrlock(&user->operations.lock);
    if (user->operations.last == NULL)
    {
        user->operations.first = user->operations.last = operation;
        operation->id = 1;
    }
    else
    {
        operation->id = user->operations.last->id + 1;
        user->operations.last->next = operation;
        operation->prev = user->operations.last;
        user->operations.last = operation;
    }
    ++user->operations.count;

    pthread_rwlock_unlock(&user->operations.lock);
    return operation;
}

void UserOperationUnregister(POnlineUser user, PUserCancelableOperation op)
{
    if (!op->cancel)
    {
        UserOperationCancel(user, op);
    }

    if (pthread_rwlock_wrlock(&user->operations.lock))
    {
        abort();
    }
    if (op->prev == NULL && op->next == NULL && user->operations.first != op)
    {
        free(op);
    }
    else
    {
        if (op->prev == NULL)
        {
            user->operations.first = op->next;
        }
        else
        {
            op->prev->next = op->next;
        }
        if (op->next == NULL)
        {
            user->operations.last = op->prev;
        }
        else
        {
            op->next->prev = op->prev;
        }

        free(op);
        --user->operations.count;
    }
    pthread_rwlock_unlock(&user->operations.lock);
}

PUserCancelableOperation UserOperationGet(POnlineUser user, uint32_t operationId)
{
    pthread_rwlock_rdlock(&user->operations.lock);
    PUserCancelableOperation ret = NULL;
    for (PUserCancelableOperation op = user->operations.first; op != NULL; op = op->next)
    {
        if (op->id == operationId)
        {
            ret = op;
            break;
        }
    }
    pthread_rwlock_unlock(&user->operations.lock);
    return ret;
}

PUserCancelableOperation UserOperationQuery(POnlineUser user, UserCancelableOperationType type, int (*func)(PUserCancelableOperation op, void *data), void *data)
{
    pthread_rwlock_rdlock(&user->operations.lock);
    PUserCancelableOperation ret = NULL;
    for (PUserCancelableOperation op = user->operations.first; op != NULL; op = op->next)
    {
        if ((type == -1 || op->type == type) && func(op, data))
        {
            ret = op;
            break;
        }
    }
    pthread_rwlock_unlock(&user->operations.lock);
    return ret;
}

int UserOperationCancel(POnlineUser user, PUserCancelableOperation op)
{
    int ret = 1;
    op->cancel = 1;
    if (op->onCancel != NULL)
    {
        ret = op->onCancel(user, op);
    }
    return ret;
}

void UserOperationRemoveAll(POnlineUser user)
{
    if (pthread_rwlock_wrlock(&user->operations.lock))
        abort();
    PUserCancelableOperation next = user->operations.first;
    user->operations.first = user->operations.last = NULL;
    for (PUserCancelableOperation op = next; op != NULL; op = next)
    {
        next = op->next;
        op->prev = op->next = NULL;
        UserOperationCancel(user, op);
    }
    user->operations.count = 0;
    pthread_rwlock_unlock(&user->operations.lock);
}

void InitUserManager()
{
    pthread_rwlock_init(&OnlineUserTable.lock, NULL);
}

void FinalizeUserManager()
{
    while (OnlineUserTable.count)
    {
        OnlineUserDelete(OnlineUserTable.first);
    }
    pthread_rwlock_destroy(&OnlineUserTable.lock);
}
