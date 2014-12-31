#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <logger.h>
#include <packets.h>
#include <unistd.h>
#include <sys/stat.h>
#include "data/user.h"
#include <server.h>
#include <sys/user.h>

pthread_rwlock_t UsersTableLock = PTHREAD_RWLOCK_INITIALIZER;
UsersTable OnlineUserTable = {
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
        [CRP_PACKET_INFO_DATA]          = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketInfoData,

        [CRP_PACKET_FRIEND__START]      = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_FRIEND_REQUEST]     = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFriendRequest,
        [CRP_PACKET_FRIEND_DATA]        = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,

        [CRP_PACKET_FILE__START]        = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_FILE_REQUEST]       = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileRequest,
        [CRP_PACKET_FILE_DATA]          = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileData,
        [CRP_PACKET_FILE_DATA_END]      = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileDataEnd,
        [CRP_PACKET_FILE_STORE_REQUEST] = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileStoreRequest,


        [CRP_PACKET_MESSAGE__START]     = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_MESSAGE_TEXT]       = (int (*)(OnlineUser *user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketMessageText,
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
    {
        return 0;
    }

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
    pthread_rwlock_init(&user->operations.lock, NULL);
    pthread_rwlock_init(&user->lock, NULL);
    pthread_mutex_init(&user->holdLock, NULL);

    user->status = OUS_PENDING_HELLO;

    pthread_rwlock_wrlock(&UsersTableLock);
    user->prev = OnlineUserTable.last;

    if (OnlineUserTable.last)
    {
        OnlineUserTable.last->next = user;
    }
    else
    {
        OnlineUserTable.first = user;
    }

    OnlineUserTable.last = user;
    OnlineUserTable.count++;
    pthread_rwlock_unlock(&UsersTableLock);

    return user;
}

int OnlineUserDelete(OnlineUser *user)
{
    pthread_rwlock_wrlock(&user->lock);
    if (user->status == OUS_PENDING_CLEAN)
    {
        pthread_rwlock_unlock(&user->lock);
        return 0;
    }
    user->status = OUS_PENDING_CLEAN;
    pthread_rwlock_unlock(&user->lock);

    while (user->holds != 0)
    {    //等待所有对用户的引用被释放
        pthread_mutex_lock(&user->holdLock);
    }

    pthread_mutex_destroy(&user->holdLock);

    UserRemoveFromPool(user);
    shutdown(user->sockfd, SHUT_RDWR);
    close(user->sockfd);
    UserFreeOnlineInfo(user);
    UserOperationRemoveAll(user);

    pthread_rwlock_destroy(&user->operations.lock);
    pthread_rwlock_destroy(&user->lock);

    pthread_rwlock_wrlock(&UsersTableLock);
    if (OnlineUserTable.first == user)
    {
        OnlineUserTable.first = user->next;
    }
    if (OnlineUserTable.last == user)
    {
        OnlineUserTable.last = user->prev;
    }
    if (user->prev)
    {
        user->prev->next = user->next;
    }
    pthread_rwlock_unlock(&UsersTableLock);
    free(user);
    return 1;
}

int OnlineUserHold(OnlineUser *user)
{
    pthread_rwlock_wrlock(&user->lock);
    if (user->status == OUS_PENDING_CLEAN)//如果用户正要被清理
    {
        pthread_rwlock_unlock(&user->lock);
        return 0;//无法保持用户信息,返回错误.
    }
    ++user->holds;
    pthread_rwlock_unlock(&user->lock);
    return 1;
}

void OnlineUserUnhold(OnlineUser *user)
{
    pthread_rwlock_wrlock(&user->lock);
    --user->holds;
    pthread_rwlock_unlock(&user->lock);
    pthread_mutex_unlock(&user->holdLock);//保持被释放.通知
}

OnlineUser *OnlineUserGet(uint32_t uid)
{
    OnlineUser *ret = NULL;
    pthread_rwlock_rdlock(&UsersTableLock);
    for (OnlineUser *user = OnlineUserTable.first; user != NULL; user = user->next)
    {
        if (user->status == OUS_ONLINE && user->info->uid == uid && OnlineUserHold(user))
        {
            ret = user;
            break;
        }
    }
    pthread_rwlock_unlock(&UsersTableLock);
    return ret;
}

void OnlineUserSetStatus(OnlineUser *user, OnlineUserStatus status)
{
    pthread_rwlock_wrlock(&user->lock);
    user->status = status;
    pthread_rwlock_unlock(&user->lock);
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

void UserFreeOnlineInfo(OnlineUser *user)
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

UserCancelableOperation *UserOperationRegister(OnlineUser *user, int type)
{
/*  removed because the client refuse to support this feature
    if (user->operations.count >= 100)
        return NULL;
*/
    pthread_rwlock_wrlock(&user->operations.lock);
    UserCancelableOperation *operation = (UserCancelableOperation *) calloc(1, sizeof(UserCancelableOperation));
    if (operation == NULL)
    {
        goto cleanup;
    }
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

void UserOperationUnregister(OnlineUser *user, UserCancelableOperation *operation)
{
    if (operation->prev == NULL && operation->next == NULL)
    {
        free(operation);
    }
    else
    {
        pthread_rwlock_wrlock(&user->operations.lock);
        for (UserCancelableOperation *op = user->operations.first; op != NULL; op = op->next)
        {
            if (op == operation)
            {
                if (op->prev == NULL)
                {
                    user->operations.first = operation->next;
                }
                else
                {
                    op->prev->next = operation->next;
                }
                if (operation->next == NULL)
                {
                    user->operations.last = operation->prev;
                }
                else
                {
                    operation->next->prev = operation->prev;
                }

                free(op);
                break;
            }
        }
        --user->operations.count;
        pthread_rwlock_unlock(&user->operations.lock);
    }
}

UserCancelableOperation *UserOperationGet(OnlineUser *user, uint32_t operationId)
{
    pthread_rwlock_rdlock(&user->operations.lock);
    UserCancelableOperation *ret = NULL;
    for (UserCancelableOperation *op = user->operations.first; op != NULL; op = op->next)
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

UserCancelableOperation *UserOperationQuery(OnlineUser *user, UserCancelableOperationType type, int (*func)(UserCancelableOperation *op, void *data), void *data)
{
    pthread_rwlock_rdlock(&user->operations.lock);
    UserCancelableOperation *ret = NULL;
    for (UserCancelableOperation *op = user->operations.first; op != NULL; op = op->next)
    {
        if (op->type == type && func(op, data))
        {
            ret = op;
            break;
        }
    }
    pthread_rwlock_unlock(&user->operations.lock);
    return ret;
}

int UserOperationCancel(OnlineUser *user, uint32_t operationId)
{
    UserCancelableOperation *op = UserOperationGet(user, operationId);

    int ret = 1;
    if (op->oncancel != NULL)
    {
        ret = op->oncancel(user, op);
    }
    else
    {
        op->cancel = 1;
    }

    return ret;
}

void UserOperationRemoveAll(OnlineUser *user)
{
    pthread_rwlock_wrlock(&user->operations.lock);
    for (UserCancelableOperation *op = user->operations.first; op != NULL; op = op->next)
    {
        if (op->oncancel != NULL)
        {
            op->oncancel(user, op);
        }
        op->cancel = 1;
        op->prev = op->next = NULL;
    }
    pthread_rwlock_unlock(&user->operations.lock);
}