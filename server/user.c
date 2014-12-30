#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <logger.h>
#include <user.h>
#include <packets.h>
#include <unistd.h>
#include <sys/stat.h>
#include <data/user.h>
#include <server.h>

pthread_rwlock_t UsersTableLock = PTHREAD_RWLOCK_INITIALIZER;
UsersTable OnlineUsers = {
        .count=0,
        .first=NULL,
        .last=NULL
};

int(*PacketsProcessMap[CRP_PACKET_ID_MAX + 1])(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header) = {
        [CRP_PACKET_KEEP_ALIVE]         = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusKeepAlive,
        [CRP_PACKET_HELLO]              = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusHello,
        [CRP_PACKET_OK]                 = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusOK,
        [CRP_PACKET_FAILURE]            = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusFailure,
        [CRP_PACKET_CRASH]              = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusCrash,

        [CRP_PACKET_LOGIN__START]       = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_LOGIN_LOGIN]        = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketLoginLogin,
        [CRP_PACKET_LOGIN_LOGOUT]       = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketLoginLogout,
        [CRP_PACKET_LOGIN_REGISTER]     = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketLoginRegister,

        [CRP_PACKET_INFO__START]        = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_INFO_REQUEST]       = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketInfoRequest,
        [CRP_PACKET_INFO_DATA]          = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,

        [CRP_PACKET_FRIEND__START]      = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_FRIEND_REQUEST]     = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFriendRequest,
        [CRP_PACKET_FRIEND_DATA]        = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,

        [CRP_PACKET_FILE__START]        = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_FILE_REQUEST]       = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileRequest,
        [CRP_PACKET_FILE_DATA]          = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileData,
        [CRP_PACKET_FILE_DATA_END]      = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileDataEnd,
        [CRP_PACKET_FILE_STORE_REQUEST] = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileStoreRequest,

        [CRP_PACKET_MESSAGE__START]     = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_MESSAGE_TEXT]       = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketMessageTextMessage,
};

/**
* Server Process User Message
*/
int ProcessUser(OnlineUser *user, CRPBaseHeader *packet)
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
        return 0;

    int(*packetProcessor)(OnlineUser *, uint32_t, void *, CRPBaseHeader *header) = PacketsProcessMap[packet->packetID];
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

OnlineUser *OnlineUserNew(int fd)
{
    OnlineUser *user = (OnlineUser *) calloc(1, sizeof(OnlineUser));

    user->sockfd = fd;
    pthread_mutex_init(&user->sockLock, NULL);
    pthread_rwlock_init(&user->operations.lock, NULL);

    user->status = OUS_PENDING_HELLO;

    pthread_rwlock_wrlock(&UsersTableLock);
    user->prev = OnlineUsers.last;

    if (OnlineUsers.last)
    {
        OnlineUsers.last->next = user;
    }
    else
    {
        OnlineUsers.first = user;
    }

    OnlineUsers.last = user;
    OnlineUsers.count++;
    pthread_rwlock_unlock(&UsersTableLock);

    return user;
}

void OnlineUserDelete(OnlineUser *user)
{
    UserRemoveFromPoll(user);
    shutdown(user->sockfd, SHUT_RDWR);
    close(user->sockfd);

    if (user->info)
    {
        if (user->info->userDir)
            free(user->info->userDir);
        free(user->info);
    }
    pthread_rwlock_wrlock(&UsersTableLock);
    if (OnlineUsers.first == user)
    {
        OnlineUsers.first = user->next;
    }
    if (OnlineUsers.last == user)
    {
        OnlineUsers.last = user->prev;
    }
    if (user->prev)
        user->prev->next = user->next;
    pthread_rwlock_unlock(&UsersTableLock);
    free(user);
}


OnlineUserInfo *UserCreateOnlineInfo(OnlineUser *user, uint32_t uid)
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
    OnlineUserInfo *info = (OnlineUserInfo *) malloc(sizeof(OnlineUser));
    memset(info, 0, sizeof(OnlineUserInfo));
    info->uid = uid;
    info->userDir = (char *) malloc(userDirSize + 1);
    memcpy(info->userDir, userDir, userDirSize);
    info->userDir[userDirSize] = 0;
    return info;
}


UserCancelableOperation *UserRegisterOperation(OnlineUser *user, int type)
{
/*  removed because the client refuse to support this feature
    if (user->operations.count >= 100)
        return NULL;
*/
    pthread_rwlock_wrlock(&user->operations.lock);
    UserCancelableOperation *operation = (UserCancelableOperation *) calloc(1,sizeof(UserCancelableOperation));
    if (operation == NULL)
        goto cleanup;
    operation->next = NULL;
    operation->type = type;
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
    cleanup:
    pthread_rwlock_unlock(&user->operations.lock);
    return operation;
}

void UserUnregisterOperation(OnlineUser *user, UserCancelableOperation *operation)
{
    pthread_rwlock_wrlock(&user->operations.lock);
    for (UserCancelableOperation *op = user->operations.first; op != user->operations.last; op = op->next)
    {
        if (op == operation)
        {
            op->prev->next = operation->next;
            operation->next->prev = operation->prev;
            if (user->operations.first == operation)
                user->operations.first = operation->next;
            if (user->operations.last == operation)
                user->operations.last = operation->prev;
            free(op);
            break;
        }
    }
    --user->operations.count;
    pthread_rwlock_unlock(&user->operations.lock);
}

UserCancelableOperation *UserGetOperation(OnlineUser *user, uint32_t operationId)
{
    pthread_rwlock_rdlock(&user->operations.lock);
    UserCancelableOperation *ret = NULL;
    for (UserCancelableOperation *op = user->operations.first; op != user->operations.last; op = op->next)
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

UserCancelableOperation *UserQueryOperation(OnlineUser *user, UserCancelableOperationType type, int (*func)(UserCancelableOperation *op, void *data), void *data) {
    pthread_rwlock_rdlock(&user->operations.lock);
    UserCancelableOperation *ret = NULL;
    for (UserCancelableOperation *op = user->operations.first; op != user->operations.last; op = op->next) {
        if (op->type == type && func(op, data)) {
            ret = op;
            break;
        }
    }
    pthread_rwlock_unlock(&user->operations.lock);
    return ret;
}

int UserCancelOperation(OnlineUser *user, uint32_t operationId) {
    UserCancelableOperation *op = UserGetOperation(user, operationId);

    int ret = 1;
    if (op->oncancel != NULL) {
        ret = op->oncancel(user, op);
    }
    else {
        op->cancel = 1;
    }

    return ret;
}
