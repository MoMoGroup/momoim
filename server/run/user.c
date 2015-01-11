#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <logger.h>
#include <sys/stat.h>
#include <run/jobs.h>
#include <errno.h>
#include <datafile/message.h>

#include "datafile/user.h"
#include "datafile/friend.h"

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
static int(*PacketsProcessMap[CRP_PACKET_ID_MAX + 1])(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header) = {
        [CRP_PACKET_KEEP_ALIVE]           = (GeneralPacketProcessor) ProcessPacketStatusKeepAlive,
        [CRP_PACKET_HELLO]                = (GeneralPacketProcessor) ProcessPacketStatusHello,
        [CRP_PACKET_OK]                   = (GeneralPacketProcessor) ProcessPacketStatusOK,
        [CRP_PACKET_FAILURE]              = (GeneralPacketProcessor) ProcessPacketStatusFailure,
        [CRP_PACKET_CRASH]                = (GeneralPacketProcessor) ProcessPacketStatusCrash,
        [CRP_PACKET_CANCEL]               = (GeneralPacketProcessor) ProcessPacketStatusCancel,
        [CRP_PACKET_SWITCH_PROTOCOL]      = (GeneralPacketProcessor) ProcessPacketStatusSwitchProtocol,

        [CRP_PACKET_LOGIN__START]         = (GeneralPacketProcessor) NULL,
        [CRP_PACKET_LOGIN_LOGIN]          = (GeneralPacketProcessor) ProcessPacketLoginLogin,
        [CRP_PACKET_LOGIN_LOGOUT]         = (GeneralPacketProcessor) ProcessPacketLoginLogout,
        [CRP_PACKET_LOGIN_REGISTER]       = (GeneralPacketProcessor) ProcessPacketLoginRegister,

        [CRP_PACKET_INFO__START]          = (GeneralPacketProcessor) NULL,
        [CRP_PACKET_INFO_REQUEST]         = (GeneralPacketProcessor) ProcessPacketInfoRequest,
        [CRP_PACKET_INFO_DATA]            = (GeneralPacketProcessor) ProcessPacketInfoData,
        [CRP_PACKET_INFO_STATUS_CHANGE]   = (GeneralPacketProcessor) ProcessPacketInfoStatusChange,

        [CRP_PACKET_FRIEND__START]        = (GeneralPacketProcessor) NULL,
        [CRP_PACKET_FRIEND_REQUEST]       = (GeneralPacketProcessor) ProcessPacketFriendRequest,
        [CRP_PACKET_FRIEND_ADD]           = (GeneralPacketProcessor) ProcessPacketFriendAdd,
        [CRP_PACKET_FRIEND_SEARCH_BY_NICKNAME]=(GeneralPacketProcessor) ProcessPacketFriendSearchByNickname,
        [CRP_PACKET_FRIEND_ACCEPT]        =  (GeneralPacketProcessor) ProcessPacketFriendAccept,
        [CRP_PACKET_FRIEND_DELETE]        = (GeneralPacketProcessor) ProcessPacketFriendDelete,
        [CRP_PACKET_FRIEND_MOVE]          = (GeneralPacketProcessor) ProcessPacketFriendMove,
        [CRP_PACKET_FRIEND_GROUP_ADD]     = (GeneralPacketProcessor) ProcessPacketFriendGroupAdd,
        [CRP_PACKET_FRIEND_GROUP_DELETE]  = (GeneralPacketProcessor) ProcessPacketFriendGroupDelete,
        [CRP_PACKET_FRIEND_GROUP_RENAME]  = (GeneralPacketProcessor) ProcessPacketFriendGroupRename,
        [CRP_PACKET_FRIEND_GROUP_MOVE]    = (GeneralPacketProcessor) ProcessPacketFriendGroupMove,


        [CRP_PACKET_FILE__START]          = (GeneralPacketProcessor) NULL,
        [CRP_PACKET_FILE_REQUEST]         = (GeneralPacketProcessor) ProcessPacketFileRequest,
        [CRP_PACKET_FILE_DATA]            = (GeneralPacketProcessor) ProcessPacketFileData,
        [CRP_PACKET_FILE_RESET]           = (GeneralPacketProcessor) ProcessPacketFileReset,
        [CRP_PACKET_FILE_DATA_END]        = (GeneralPacketProcessor) ProcessPacketFileDataEnd,
        [CRP_PACKET_FILE_STORE_REQUEST]   = (GeneralPacketProcessor) ProcessPacketFileStoreRequest,


        [CRP_PACKET_MESSAGE__START]       = (GeneralPacketProcessor) NULL,
        [CRP_PACKET_MESSAGE_NORMAL]       = (GeneralPacketProcessor) ProcessPacketMessageNormal,
        [CRP_PACKET_MESSAGE_QUERY_OFFLINE]= (GeneralPacketProcessor) ProcessPacketMessageQueryOffline,

        [CRP_PACKET_NET__START]           = (GeneralPacketProcessor) NULL,
        [CRP_PACKET_NAT_DISCOVER]         = (GeneralPacketProcessor) ProcessPacketNatDiscover,
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
    int(*packetProcessor)(POnlineUser, uint32_t, void *, CRPBaseHeader *) = PacketsProcessMap[packet->packetID];
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

static void PendingUserTableRemove(PPendingUser user)
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
    if (user->state == OUS_ONLINE)
        return OnlineUserDelete((POnlineUser) user);
    else if (user->state == OUS_PENDING_CLEAN)
        return 0;
    UserSetStatus((POnlineUser) user, OUS_PENDING_CLEAN, NULL);
    pthread_rwlock_unlock(user->holdLock);
    pthread_rwlock_wrlock(user->holdLock);
    PendingUserTableRemove(user);
    pthread_rwlock_unlock(user->holdLock);
    pthread_rwlock_destroy(user->holdLock);
    free(user->holdLock);
    free(user);
    return 1;
}

static POnlineUser OnlineUserGetUnlock(uint32_t uid)
{
    uint32_t current = uid;
    int reserve[sizeof(current) * 2];
    int end = 0;
    while (current)
    {
        reserve[end++] = current & 0xf;
        current >>= 4;
    }
    --end;
    OnlineUsersTableType *currentTable = &OnlineUserTable;
    while (end >= 0)
    {
        if (currentTable->next[reserve[end]] == NULL)
        {
            return NULL;
        }
        currentTable = currentTable->next[reserve[end]];
        --end;
    }
    if (currentTable->user && UserHold(currentTable->user))
        return currentTable->user;
    else
        return NULL;
}

static POnlineUser OnlineUserTableSetUnlock(uint32_t uid, OnlineUser *user)
{
    uint32_t current = uid;
    int reserve[sizeof(current) * 2];
    int end = 0;
    while (current)
    {
        reserve[end++] = current & 0xf;
        current >>= 4;
    }
    --end;
    POnlineUsersTableType currentTable = &OnlineUserTable;
    while (end >= 0)
    {
        if (currentTable->next[reserve[end]] == NULL)
        {
            currentTable->next[reserve[end]] = calloc(1, sizeof(OnlineUsersTableType));
        }
        currentTable = currentTable->next[reserve[end]];
        --end;
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

POnlineUser OnlineUserTableSet(uint32_t uid, OnlineUser *user)
{
    POnlineUser ret = NULL;
    pthread_rwlock_wrlock(&OnlineUserTableLock);
    ret = OnlineUserTableSetUnlock(uid, user);
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
    user->sockfd = CRPOpen(fd);
    user->state = OUS_PENDING_HELLO;
    user->holdLock = (pthread_rwlock_t *) malloc(sizeof(pthread_rwlock_t));
    pthread_rwlock_init(user->holdLock, NULL);
    time(&user->lastUpdateTime);
    return user;
}

void UserBroadcastNotify(POnlineUser user, FriendNotifyType type)
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
                if (duser->state == OUS_ONLINE)
                {
                    CRPFriendNotifySend(duser->crp, 0, type, info->uid, 0, 0);
                }
                UserDrop(duser);
            }
        }
    }
    pthread_rwlock_unlock(info->friendsLock);
}

int OnlineUserDelete(POnlineUser user)
{
    if (user->state == OUS_PENDING_HELLO || user->state == OUS_PENDING_LOGIN)
        return PendingUserDelete((PPendingUser) user);
    if (user->state == OUS_PENDING_CLEAN)
        return 1;
    if (user->state != OUS_ONLINE)
    {
        log_error("UserManager", "Trying to delete online user on illegal user state.\n");
        return 0;
    }
    if (UserSetStatus(user, OUS_PENDING_CLEAN, NULL) == NULL)
        return 0;
    pthread_rwlock_unlock(user->holdLock);

    pthread_rwlock_wrlock(user->holdLock);
    UserOperationRemoveAll(user);
    pthread_mutex_destroy(&user->operations.lock);
    CRPClose(user->crp);
    if (user->info)
    {
        log_info("UserManager", "User %d offline.\n", user->info->uid);
        UserInfo *info = UserInfoGet(user->info->uid);
        if (info)
        {
            time_t now;
            time(&now);
            info->lastlogout = now;
            info->level += (now - user->info->loginTime) / 60;
            UserInfoSave(info->uid, info);
            UserInfoFree(info);
        }

        UserFriendsDrop(user->info->uid);

        if (user->info->userDir)
        {
            free(user->info->userDir);
        }
        free(user->info);
        user->info = NULL;
    }

    pthread_rwlock_unlock(user->holdLock);
    pthread_rwlock_destroy(user->holdLock);
    free(user->holdLock);

    free(user);
    return 1;
}

int UserHold(POnlineUser user)
{
    return user->state != OUS_PENDING_CLEAN && pthread_rwlock_tryrdlock(user->holdLock) == 0;
}

void UserDrop(POnlineUser user)
{
    if (user)
        pthread_rwlock_unlock(user->holdLock);
}

POnlineUser UserSetStatus(POnlineUser user, OnlineUserState state, uint32_t uid)
{
    if (user->state == OUS_PENDING_CLEAN)
        //正在清理的用户内存区域可能正在释放.此时不应改变状态.
        return NULL;
    if (user->state == OUS_PENDING_HELLO && state == OUS_PENDING_LOGIN)
    {//等待Hello到等待登陆只需要设置标识位
        user->state = OUS_PENDING_LOGIN;
    }
    else if (user->state == OUS_PENDING_LOGIN && state == OUS_ONLINE)
    {   //待登陆状态切换到在线状态
        PPendingUser pendingUser = (PPendingUser) user;
        PendingUserTableRemove(pendingUser);
        void *ret = realloc(user, sizeof(OnlineUser));
        if (!ret)//无法重新分配内存,状态切换失败
        {   //为保护服务器,直接删除状态错误的用户
            PendingUserDelete(pendingUser);
            return NULL;
        }
        JobManagerKick(user);
        user = ret;
        EpollModify(user);
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

        info->friends = UserFriendsGet(uid, &info->friendsLock, -1);
        user->info = info;
        bzero(&user->operations, sizeof(UserOperationTable));
        //初始化用户操作锁
        pthread_mutex_init(&user->operations.lock, NULL);
        user->state = OUS_ONLINE;
        UserBroadcastNotify(user, FNT_FRIEND_ONLINE);
        return OnlineUserTableSet(info->uid, user);
    }
    else if (user->state == OUS_ONLINE && state == OUS_PENDING_CLEAN)
    {
        OnlineUserTableSet(user->info->uid, NULL);
        EpollRemove(user);
        JobManagerKick(user);
        if (user->info->status != UOS_HIDDEN)
        {
            UserBroadcastNotify(user, FNT_FRIEND_OFFLINE);
        }
    }
    else if (state == OUS_PENDING_CLEAN)
    {
        user->state = OUS_PENDING_CLEAN;
    }
    else
    {
        log_warning("UserManager", "Illegal state set. Orginal %d,New %d.\n", user->state, state);
        abort();
    }
    return user;
}

PUserOperation UserOperationRegister(POnlineUser user, session_id_t sessionID, int type, void *data)
{
    /* It will cause some trouble */
    if (user->operations.count >= 1000)
        return NULL;

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
    op->cancel = 1;
    if (op->onCancel != NULL)
    {
        if (op->onCancel(user, op))
            UserOperationDrop(user, op);
    }
    else
    {
        UserOperationUnregister(user, op);
    }
    return 1;
}

void UserOperationRemoveAll(POnlineUser user)
{
    if (pthread_mutex_lock(&user->operations.lock))
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
    pthread_mutex_unlock(&user->operations.lock);
}


void PostMessage(UserMessage *message)
{
    MessageFile *file = UserMessageFileGet(message->to);
    if (file)
    {
        MessageFileAppend(file, message);
        UserMessageFileDrop(message->to);
    }
    POnlineUser toUser = OnlineUserGet(message->to);
    if (toUser != NULL)
    {
        log_info("PostMessager", "Post message from %u to %u\n", message->from, message->to);
        CRPMessageNormalSend(toUser->crp,
                             0,
                             (USER_MESSAGE_TYPE) message->messageType,
                             message->from,
                             message->messageLen,
                             message->content);
        UserDrop(toUser);
    }
    else
    {
        log_info("PostMessager", "Post offline message from %u to %u\n", message->from, message->to);
    }

}