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

//消息处理器映射表
int(*PacketsProcessMap[CRP_PACKET_ID_MAX + 1])(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header) = {
        [CRP_PACKET_KEEP_ALIVE]         = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusKeepAlive,
        [CRP_PACKET_HELLO]              = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusHello,
        [CRP_PACKET_OK]                 = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusOK,
        [CRP_PACKET_FAILURE]            = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusFailure,
        [CRP_PACKET_CRASH]              = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusCrash,
        [CRP_PACKET_CANCEL]             = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketStatusCancel,

        [CRP_PACKET_LOGIN__START]       = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_LOGIN_LOGIN]        = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketLoginLogin,
        [CRP_PACKET_LOGIN_LOGOUT]       = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketLoginLogout,
        [CRP_PACKET_LOGIN_REGISTER]     = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketLoginRegister,

        [CRP_PACKET_INFO__START]        = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_INFO_REQUEST]       = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketInfoRequest,
        [CRP_PACKET_INFO_DATA]          = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketInfoData,

        [CRP_PACKET_FRIEND__START]      = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_FRIEND_REQUEST]     = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFriendRequest,
        [CRP_PACKET_FRIEND_ADD]         = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFriendAdd,
        [CRP_PACKET_FRIEND_SEARCH_BY_NICKNAME]=(int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFriendSearchByNickname,

        [CRP_PACKET_FILE__START]        = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_FILE_REQUEST]       = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileRequest,
        [CRP_PACKET_FILE_DATA]          = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileData,
        [CRP_PACKET_FILE_RESET]         = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileReset,
        [CRP_PACKET_FILE_DATA_END]      = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileDataEnd,
        [CRP_PACKET_FILE_STORE_REQUEST] = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketFileStoreRequest,


        [CRP_PACKET_MESSAGE__START]     = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) NULL,
        [CRP_PACKET_MESSAGE_NORMAL]       = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketMessageText,
};

int ProcessUser(POnlineUser user, CRPBaseHeader *packet)
{
    int ret = 1;
    void *data;
    //查找解码器
    void *(*packetCast)(CRPBaseHeader *) = PacketsDataCastMap[packet->packetID];
    if (packetCast == NULL)//如果诶有找到,注销当前用户
    {
        return 0;
    }
    data = packetCast(packet);//尝试解码,
    if (data == NULL)//如果解码失败,注销用户
    {
        return 0;
    }

    //查找处理机
    int(*packetProcessor)(POnlineUser, uint32_t, void *, CRPBaseHeader *header) = PacketsProcessMap[packet->packetID];
    if (packetProcessor != NULL)//如果找到,处理数据包
    {
        ret = packetProcessor(user, packet->sessionID, data, packet);
    }
    else
    {
        log_warning("UserProc", "Packet %d has no handler.\n", packet->packetID);
    }

    if (data != packet->data)//如果解包分配的内存区域是新分配的,则释放这块内存
    {
        free(data);
    }
    return ret;
}

POnlineUser OnlineUserNew(int fd)
{
    POnlineUser user = (POnlineUser) calloc(1, sizeof(OnlineUser));//用户基本数据需要占用内存空间
    if (user == NULL)
    {
        log_error("UserManager", "Fail to calloc new user.\n");
        return NULL;
    }
    //简要设置一下socket,状态.初始化用户保持锁
    user->sockfd = fd;
    user->status = OUS_PENDING_INIT;
    pthread_rwlock_init(&user->holdLock, NULL);

    return user;
}

void OnlineUserInit(POnlineUser user)
{
    //初始化用户信息
    if (!user->status == OUS_PENDING_INIT)
        return;
    //初始化用户操作锁
    pthread_rwlock_init(&user->operations.lock, NULL);
    //用户当前可以接收HELLO数据包了
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
    if (user)
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


int UserCreateOnlineInfo(POnlineUser user, uint32_t uid)
{
    char path[30];
    uint8_t userDirSize;
    UserGetDir(path, uid, "");
    userDirSize = (uint8_t) strlen(path);
    struct stat buf;
    if (stat(path, &buf) || !S_ISDIR(buf.st_mode))
    {
        UserCreateDirectory(uid);
    }
    POnlineUserInfo info = (POnlineUserInfo) calloc(1, sizeof(OnlineUser));
    if (info == NULL)
    {
        return 0;
    }
    info->uid = uid;
    info->userDir = (char *) malloc(userDirSize + 1);
    if (info->userDir == NULL)
    {
        free(info);
        return 0;
    }
    memcpy(info->userDir, path, userDirSize);
    info->userDir[userDirSize] = 0;

    info->friends = UserGetFriends(uid);
    user->info = info;
    return 1;
}

void UserFreeOnlineInfo(POnlineUser user)
{
    if (user->info)
    {
        log_info("UserManager", "User %d offline.\n", user->info->uid);
        for (int i = 0; i < user->info->friends->groupCount; ++i)
        {
            UserGroup *group = user->info->friends->groups + i;
            for (int j = 0; j < group->friendCount; ++j)
            {
                POnlineUser duser = OnlineUserGet(group->friends[j]);
                if (duser)
                {
                    if (duser->status == OUS_ONLINE)
                    {
                        CRPFriendNotifySend(duser->sockfd, 0, user->info->uid, FNT_OFFLINE);
                    }
                    OnlineUserDrop(duser);
                }
            }
        }
        if (user->info->friends)
        {
            UserSaveFriendsFile(user->info->uid, user->info->friends);
            UserFreeFriends(user->info->friends);
        }
        if (user->info->userDir)
        {
            free(user->info->userDir);
        }
        free(user->info);
        user->info = NULL;
    }
}

PUserOperation UserOperationRegister(POnlineUser user, session_id_t sessionID, int type, void *data)
{
/*  the can not support this feature
    if (user->operations.count >= 100)
        return NULL;
*/
    PUserOperation operation = (PUserOperation) calloc(1, sizeof(UserOperation));
    if (operation == NULL)
    {
        return NULL;
    }
    operation->next = NULL;
    operation->type = type;
    operation->session = sessionID;
    operation->data = data;
    pthread_mutex_init(&operation->lock, NULL);
    pthread_rwlock_wrlock(&user->operations.lock);

    if (user->operations.last == NULL)
    {
        user->operations.first = user->operations.last = operation;
    }
    else
    {
        user->operations.last->next = operation;
        operation->prev = user->operations.last;
        user->operations.last = operation;
    }
    ++user->operations.count;

    pthread_rwlock_unlock(&user->operations.lock);
    pthread_mutex_lock(&operation->lock);
    return operation;
}

void UserOperationUnregister(POnlineUser user, PUserOperation op)
{
    if (!op->cancel)
    {
        UserOperationCancel(user, op);
        return;
    }

    if (op->prev == NULL && op->next == NULL && user->operations.first != op)
    {
        pthread_mutex_unlock(&op->lock);
        pthread_mutex_destroy(&op->lock);
        free(op);
    }
    else
    {

        pthread_rwlock_wrlock(&user->operations.lock);
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

        pthread_mutex_unlock(&op->lock);
        pthread_mutex_destroy(&op->lock);
        free(op);
        --user->operations.count;
        pthread_rwlock_unlock(&user->operations.lock);
    }
}

PUserOperation UserOperationGet(POnlineUser user, uint32_t sessionId)
{
    pthread_rwlock_rdlock(&user->operations.lock);
    PUserOperation ret = NULL;
    for (PUserOperation op = user->operations.first; op != NULL; op = op->next)
    {
        if (op->session == sessionId)
        {
            ret = op;
            break;
        }
    }
    pthread_rwlock_unlock(&user->operations.lock);
    if (ret)
    {
        if (pthread_mutex_trylock(&ret->lock))
        {
            return NULL;
        }
    }
    return ret;
}

void UserOperationDrop(PUserOperation op)
{
    pthread_mutex_unlock(&op->lock);
}

PUserOperation UserOperationQuery(POnlineUser user, UserOperationType type, int (*func)(PUserOperation op, void *data), void *data)
{
    pthread_rwlock_rdlock(&user->operations.lock);
    PUserOperation ret = NULL;
    for (PUserOperation op = user->operations.first; op != NULL; op = op->next)
    {
        if ((type == -1 || op->type == type) && func(op, data))
        {
            ret = op;
            break;
        }
    }
    pthread_rwlock_unlock(&user->operations.lock);
    if (ret)
    {
        if (pthread_mutex_trylock(&ret->lock))
        {
            return NULL;
        }
    }
    return ret;
}

int UserOperationCancel(POnlineUser user, PUserOperation op)
{
    int ret = 1;
    op->cancel = 1;
    if (op->onCancel != NULL)
    {
        ret = op->onCancel(user, op);
    }
    else
    {
        UserOperationUnregister(user, op);
    }
    return ret;
}

void UserOperationRemoveAll(POnlineUser user)
{
    if (pthread_rwlock_wrlock(&user->operations.lock))
        abort();
    PUserOperation next = user->operations.first;
    user->operations.first = user->operations.last = NULL;
    for (PUserOperation op = next; op != NULL; op = next)
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

int PostMessage(UserMessage *message)
{
    int ret;
    POnlineUser toUser = OnlineUserGet(message->to);
    if (toUser == NULL)
    {
        MessageFile *file = UserMessageFileOpen(message->to);
        ret = MessageFileAppend(file, message);
        MessageFileClose(file);
    }
    else
    {
        CRPMessageNormalSend(toUser->sockfd, 0, (USER_MESSAGE_TYPE) message->messageType, message->from, message->messageLen, message->content);
        OnlineUserDrop(toUser);
        ret = 1;
    }
    return ret;
}