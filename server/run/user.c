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
#include<errno.h>

OnlineUsersTableType OnlineUserTable = {
        .user=NULL,
        .prev=NULL
};
PendingUsersTableType PendingUserTable = {
        .first=NULL,
        .last=NULL
};
pthread_rwlock_t OnlineUserTableLock, PendingUserTableLock;

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
        [CRP_PACKET_MESSAGE_NORMAL]     = (int (*)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header)) ProcessPacketMessageNormal,
};

void InitUserManager()
{
    pthread_rwlock_init(&OnlineUserTableLock, NULL);
    pthread_rwlock_init(&PendingUserTableLock, NULL);
}

void FinalizeUserManager()
{
    while (PendingUserTable.first)
    {
        PendingUserDelete(PendingUserTable.first);
    }
    pthread_rwlock_destroy(&OnlineUserTableLock);
    pthread_rwlock_destroy(&PendingUserTableLock);
}

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

static void PendingUserRemove(PPendingUser user)
{
    pthread_rwlock_wrlock(&PendingUserTableLock);
    if (user->prev != NULL || user->next != NULL || PendingUserTable.first == user)
    {
        if (user->prev == NULL)
        {
            PendingUserTable.first = user->next;
        }
        else
        {
            user->prev->next = user->next;
        }
        if (user->next == NULL)
        {
            PendingUserTable.last = user->prev;
        }
        else
        {
            user->next->prev = user->prev;
        }
        user->prev = user->next = NULL;
    }
    pthread_rwlock_unlock(&PendingUserTableLock);
}

int PendingUserDelete(PPendingUser user)
{
    if (user->status == OUS_ONLINE)
        return OnlineUserDelete((POnlineUser) user);
    else if (user->status == OUS_PENDING_CLEAN)
        return 0;
    OnlineUserSetStatus((POnlineUser) user, OUS_PENDING_CLEAN, NULL);
    pthread_rwlock_unlock(user->holdLock);
    pthread_rwlock_wrlock(user->holdLock);
    PendingUserRemove(user);
    free(user);
    return 1;
}

static POnlineUser OnlineUserGetUnlock(uint32_t uid)
{
    uint32_t current = uid;
    OnlineUsersTableType *currentTable = &OnlineUserTable;
    while (current)
    {
        if (currentTable->next[current & 0xf] == NULL)
        {
            return NULL;
        }
        currentTable = currentTable->next[current & 0xf];
        current >>= 4;
    }
    if (currentTable->user && UserHold(currentTable->user))
        return currentTable->user;
    else
        return NULL;
}

static POnlineUser OnlineUserSetUnlock(uint32_t uid, OnlineUser *user)
{
    uint32_t current = uid;
    POnlineUsersTableType currentTable = &OnlineUserTable;
    while (current)
    {
        if (currentTable->next[current & 0xf] == NULL)
        {
            currentTable->next[current & 0xf] = calloc(1, sizeof(OnlineUsersTableType));
        }
        currentTable = currentTable->next[current & 0xf];
        current >>= 4;
    }
    currentTable->user = user;
    return user;
}

POnlineUser OnlineUserGet(uint32_t uid)
{
    POnlineUser ret = NULL;
    pthread_rwlock_rdlock(&OnlineUserTableLock);
    ret = OnlineUserGetUnlock(uid);
    pthread_rwlock_unlock(&OnlineUserTableLock);
    return ret;
}

POnlineUser OnlineUserSet(uint32_t uid, OnlineUser *user)
{
    POnlineUser ret = NULL;
    pthread_rwlock_wrlock(&OnlineUserTableLock);
    ret = OnlineUserSetUnlock(uid, user);
    pthread_rwlock_unlock(&OnlineUserTableLock);
    return ret;
}

PPendingUser PendingUserNew(int fd)
{
    PPendingUser user = (PPendingUser) calloc(1, sizeof(PendingUser));
    if (user == NULL)
    {
        log_error("UserManager", "Fail to calloc new user.\n");
        return NULL;
    }
    //简要设置一下socket,状态.初始化用户保持锁
    user->sockfd = fd;
    user->status = OUS_PENDING_HELLO;
    user->holdLock = (pthread_rwlock_t *) malloc(sizeof(pthread_rwlock_t));
    pthread_rwlock_init(user->holdLock, NULL);

    return user;
}

int OnlineUserDelete(POnlineUser user)
{
    if (user->status == OUS_PENDING_HELLO || user->status == OUS_PENDING_CLEAN)
        return PendingUserDelete((PPendingUser) user);
    if (user->status == OUS_PENDING_CLEAN)
        return 1;
    if (user->status != OUS_ONLINE)
    {
        log_error("UserManager", "Illegal user status.\n");
        return 0;
    }
    if (OnlineUserSetStatus(user, OUS_PENDING_CLEAN, NULL) == NULL)
        return 0;
    uint32_t uid = user->info->uid;
    pthread_rwlock_unlock(user->holdLock);
    OnlineUserSet(uid, NULL);

    pthread_rwlock_wrlock(user->holdLock);
    JobManagerKick(user);
    EpollRemove(user);
    UserOperationRemoveAll(user);
    pthread_mutex_destroy(&user->operations.lock);

    shutdown(user->sockfd, SHUT_RDWR);
    close(user->sockfd);
    UserFreeOnlineInfo(user);


    pthread_rwlock_unlock(user->holdLock);
    pthread_rwlock_destroy(user->holdLock);
    free(user->holdLock);

    free(user);
    return 1;
}

int UserHold(POnlineUser user)
{
    return user->status != OUS_PENDING_CLEAN && pthread_rwlock_tryrdlock(user->holdLock) == 0;
}

void UserDrop(POnlineUser user)
{
    if (user)
        pthread_rwlock_unlock(user->holdLock);
}

static void broadcastNotify(POnlineUser user, FriendNotifyType type)
{
    POnlineUserInfo info = user->info;
    pthread_rwlock_rdlock(info->friendsLock);
    OnlineUser *duser;
    for (int i = 0; i < info->friends->groupCount; ++i)
    {
        UserGroup *group = info->friends->groups + i;
        if (group->groupId == UGI_BLACKLIST || group->groupId == UGI_PENDING)
            continue;
        for (int j = 0; j < group->friendCount; ++j)
        {
            if (group->friends[j] == info->uid)
                continue;
            duser = OnlineUserGet(group->friends[j]);
            if (duser)
            {
                if (duser->status == OUS_ONLINE)
                {
                    CRPFriendNotifySend(duser->sockfd, 0, info->uid, type);
                }
                UserDrop(duser);
            }
        }
    }
    pthread_rwlock_unlock(info->friendsLock);
}

POnlineUser OnlineUserSetStatus(POnlineUser user, OnlineUserStatus status, POnlineUserInfo info)
{
    if (user->status == OUS_PENDING_CLEAN)
        return NULL;
    if (user->status == OUS_PENDING_HELLO && status == OUS_PENDING_LOGIN)
    {//等待Hello到等待登陆只需要设置标识位
        user->status = OUS_PENDING_LOGIN;
    }
    else if (user->status == OUS_PENDING_LOGIN && status == OUS_ONLINE)
    {   //待登陆状态切换到在线状态
        PPendingUser pendingUser = (PPendingUser) user;
        PendingUserRemove(pendingUser);
        void *ret = realloc(user, sizeof(OnlineUser));
        if (!ret)//无法重新分配内存,状态切换失败
        {   //为保护服务器,直接删除状态错误的用户
            PendingUserDelete(pendingUser);
            return NULL;
        }
        JobManagerKick(user);
        user = ret;
        EpollModify(user);
        user->info = info;
        bzero(&user->operations, sizeof(UserOperationTable));
        //初始化用户操作锁
        pthread_mutex_init(&user->operations.lock, NULL);
        user->status = OUS_ONLINE;
        broadcastNotify(user, FNT_ONLINE);
        return OnlineUserSet(info->uid, user);
    }
    else if (user->status == OUS_ONLINE && status == OUS_PENDING_CLEAN)
    {
        broadcastNotify(user, FNT_OFFLINE);
    }
    else if (status == OUS_PENDING_CLEAN)
    {
        user->status = OUS_PENDING_CLEAN;
    }
    else
    {
        log_warning("UserManager", "Illegal status set. Orginal %d,New %d.\n", user->status, status);
        abort();
    }
    return user;
}


POnlineUser UserSwitchToOnline(PPendingUser user, uint32_t uid)
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

    info->friends = UserFriendsGet(uid, &info->friendsLock);
    return OnlineUserSetStatus((POnlineUser) user, OUS_ONLINE, info);
}

void UserFreeOnlineInfo(POnlineUser user)
{
    if (user->info)
    {
        log_info("UserManager", "User %d offline.\n", user->info->uid);
        for (int i = 0; i < user->info->friends->groupCount; ++i)
        {
            UserGroup *group = user->info->friends->groups + i;
            if (group->groupId == UGI_BLACKLIST || group->groupId == UGI_PENDING)
                continue;
            for (int j = 0; j < group->friendCount; ++j)
            {
                POnlineUser duser = OnlineUserGet(group->friends[j]);
                if (duser)
                {
                    if (duser->status == OUS_ONLINE)
                    {
                        CRPFriendNotifySend(duser->sockfd, 0, user->info->uid, FNT_OFFLINE);
                    }
                    UserDrop(duser);
                }
            }
        }
        UserFriendsDrop(user->info->uid);

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
    pthread_mutex_lock(&user->operations.lock);

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

    pthread_mutex_unlock(&user->operations.lock);
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

    pthread_mutex_lock(&user->operations.lock);
    if (op->prev == NULL && op->next == NULL && user->operations.first != op)
    {
        pthread_mutex_unlock(&op->lock);
        pthread_mutex_destroy(&op->lock);
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
        --user->operations.count;
        pthread_mutex_unlock(&op->lock);
        pthread_cond_broadcast(&user->operations.unlockCond);
        pthread_mutex_destroy(&op->lock);
        free(op);
    }
    pthread_mutex_unlock(&user->operations.lock);
}

PUserOperation UserOperationGet(POnlineUser user, uint32_t sessionId)
{
    pthread_mutex_lock(&user->operations.lock);
    PUserOperation ret;
    int errcode;
    refind:
    ret = NULL;
    for (PUserOperation op = user->operations.first; op != NULL; op = op->next)
    {
        if (op->session == sessionId)
        {
            ret = op;
            break;
        }
    }
    if (ret)
    {
        errcode = pthread_mutex_trylock(&ret->lock);
        if (0 != errcode)
        {
            if (errcode == EBUSY)
                //本机测试pthread_mutex_trylock返回非0值的时候,errno返回竟然是0.Unbelievable!
            {
                pthread_cond_wait(&user->operations.unlockCond, &user->operations.lock);
                goto refind;
            }
            else
            {
                ret = NULL;
            }
        }
    }
    pthread_mutex_unlock(&user->operations.lock);
    return ret;
}

void UserOperationDrop(POnlineUser user, PUserOperation op)
{
    pthread_mutex_unlock(&op->lock);
    pthread_cond_broadcast(&user->operations.unlockCond);
}

PUserOperation UserOperationQuery(POnlineUser user, UserOperationType type, int (*func)(PUserOperation op, void *data), void *data)
{
    pthread_mutex_lock(&user->operations.lock);
    PUserOperation ret = NULL;
    for (PUserOperation op = user->operations.first; op != NULL; op = op->next)
    {
        if ((type == -1 || op->type == type) && func(op, data))
        {
            ret = op;
            break;
        }
    }
    pthread_mutex_unlock(&user->operations.lock);
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


int PostMessage(UserMessage *message)
{
    POnlineUser toUser = OnlineUserGet(message->to);
    if (toUser == NULL)
    {
        MessageFile *file = UserMessageFileOpen(message->to);
        int ret = MessageFileAppend(file, message);
        MessageFileClose(file);
        return ret;
    }
    else
    {
        CRPMessageNormalSend(toUser->sockfd, 0, (USER_MESSAGE_TYPE) message->messageType, message->from, message->messageLen, message->content);
        UserDrop(toUser);
        return 1;
    }
}