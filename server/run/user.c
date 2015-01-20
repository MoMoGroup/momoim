#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <logger.h>
#include <sys/stat.h>
#include <run/jobs.h>
#include <datafile/message.h>
#include <asm-generic/errno.h>

#include "datafile/user.h"
#include "datafile/friend.h"

OnlineUsersTableType OnlineUserTable = {
        .user=(POnlineUser) -1,
        .prev=NULL
};
uint_fast32_t OnlineUserCount = 0;
PendingUsersTableType PendingUserTable = {
        .first=NULL,
        .last=NULL
};
static pthread_mutex_t OnlineUserTableLock, PendingUserTableLock;

//消息处理器映射表
static int(*PacketsProcessMap[CRP_PACKET_ID_MAX + 1])(POnlineUser, uint32_t, void *, CRPBaseHeader *) = {
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
        [CRP_PACKET_INFO_PASSWORD_CHANGE] = (GeneralPacketProcessor) ProcessPacketInfoPasswordChange,

        [CRP_PACKET_FRIEND__START]        = (GeneralPacketProcessor) NULL,
        [CRP_PACKET_FRIEND_REQUEST]       = (GeneralPacketProcessor) ProcessPacketFriendRequest,
        [CRP_PACKET_FRIEND_ADD]           = (GeneralPacketProcessor) ProcessPacketFriendAdd,
        [CRP_PACKET_FRIEND_SEARCH_BY_NICKNAME]=(GeneralPacketProcessor) ProcessPacketFriendSearchByNickname,
        [CRP_PACKET_FRIEND_ACCEPT]        = (GeneralPacketProcessor) ProcessPacketFriendAccept,
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
        [CRP_PACKET_MESSAGE_RECORD_QUERY] = (GeneralPacketProcessor) ProcessPacketMessageRecordQuery,

        [CRP_PACKET_NET__START]           = (GeneralPacketProcessor) NULL,
        [CRP_PACKET_NET_FRIEND_DISCOVER]  = (GeneralPacketProcessor) ProcessPacketNETFriendDiscover,
        [CRP_PACKET_NET_NAT_REGISTER]     = (GeneralPacketProcessor) ProcessPacketNETNATRegister,
        [CRP_PACKET_NET_NAT_REQUEST]      = (GeneralPacketProcessor) ProcessPacketNETNATRequest,
        [CRP_PACKET_NET_NAT_ACCEPT]       = (GeneralPacketProcessor) ProcessPacketNETNATAccept,
        [CRP_PACKET_NET_NAT_REFUSE]       = (GeneralPacketProcessor) ProcessPacketNETNATRefuse,
        [CRP_PACKET_NET_DISCOVER_ACCEPT]  = (GeneralPacketProcessor) ProcessPacketNetDiscoverAccept,
        [CRP_PACKET_NET_DISCOVER_REFUSE]  = (GeneralPacketProcessor) ProcessPacketNetDiscoverRefuse,
        [CRP_PACKET_NET_NAT_READY]        = (GeneralPacketProcessor) ProcessPacketNETNATReady
};

void InitUserManager()
{
    pthread_mutexattr_t recursiveAttr;
    pthread_mutexattr_init(&recursiveAttr);
    pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&OnlineUserTableLock, &recursiveAttr);
    pthread_mutex_init(&PendingUserTableLock, &recursiveAttr);
}

static void cleanupUserTable(POnlineUsersTableType table)
{
    for (int i = 0; i < 0x10; ++i)
    {
        if (table->next[i])
        {
            cleanupUserTable(table->next[i]);
        }
        free(table->next[i]);
    }
    if (table->user != (POnlineUser) -1)
    {
        free(table);
    }
}

static void deleteAllUser()
{
    pthread_mutex_lock(&OnlineUserTableLock);
    cleanupUserTable(&OnlineUserTable);
    pthread_mutex_unlock(&OnlineUserTableLock);
}

void UserManagerFinalize()
{
    while (PendingUserTable.first)
    {
        PendingUserDelete(PendingUserTable.first);
    }
    deleteAllUser();
    pthread_mutex_destroy(&OnlineUserTableLock);
    pthread_mutex_destroy(&PendingUserTableLock);
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
    int lock;
    lock = pthread_mutex_lock(&PendingUserTableLock);
    if (lock != 0 && lock != EDEADLOCK)
    {
        return;
    }
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
    if (lock == 0)
    {
        pthread_mutex_unlock(&PendingUserTableLock);
    }
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
    pthread_mutex_lock(&OnlineUserTableLock);
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
            goto cleanup;
        }
        currentTable = currentTable->next[reserve[end]];
        --end;
    }
    if (currentTable->user
        && currentTable != &OnlineUserTable
        && OnlineUserHold(currentTable->user))
    {
        ret = currentTable->user;
    }

    cleanup:
    pthread_mutex_unlock(&OnlineUserTableLock);
    return ret;
}

void OnlineUserTableRemove(uint32_t uid)
{
    pthread_mutex_lock(&OnlineUserTableLock);
    --OnlineUserCount;
    OnlineUserTableSetUnlock(uid, NULL);
    pthread_mutex_unlock(&OnlineUserTableLock);
}

POnlineUser OnlineUserTableSet(OnlineUser *user)
{
    POnlineUser ret = NULL;
    pthread_mutex_lock(&OnlineUserTableLock);
    ret = OnlineUserTableSetUnlock(user->uid, user);
    ++OnlineUserCount;
    pthread_mutex_unlock(&OnlineUserTableLock);
    return ret;
}

PPendingUser UserNew(int fd)
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
    time(&user->timeLastPacket);
    return user;
}

static int OnlineTableGC(POnlineUsersTableType table)
{
    time_t now;
    time(&now);
    int child = 0;
    for (int i = 0; i < 0x10; ++i)
    {
        if (table->next[i])
        {
            if (OnlineTableGC(table->next[i]))
            {
                table->next[i] = NULL;
            }
            else
            {
                child++;
            }
        }
    }

    if (!child)
    {
        if (!table->user)
        {
            free(table);
            return 1;
        }
        else if (table->user != (void *) -1)
        {
            OnlineUserHold(table->user);
            if (difftime(table->user->lastUpdateTime, now) > 120)//Online用户最小宽限时间为120秒
            {
                OnlineUserDelete(table->user);
            }
            else
            {
                UserDrop(table->user);
            }
        }
    }
    return 0;
}

void UserGC()
{
    pthread_mutex_lock(&PendingUserTableLock);
    time_t now;
    time(&now);
    for (PPendingUser user = PendingUserTable.first; user != NULL; user = user->next)
    {
        if (difftime(now, user->timeLastPacket) > 10)//Pending表最小宽限时间为10秒
        {
            PendingUserDelete(user);
        }
    }
    pthread_mutex_unlock(&PendingUserTableLock);

    pthread_mutex_lock(&OnlineUserTableLock);
    OnlineTableGC(&OnlineUserTable);
    pthread_mutex_unlock(&OnlineUserTableLock);
}

int OnlineUserHold(POnlineUser user)
{
    return user && (user->state != OUS_ONLINE || pthread_rwlock_tryrdlock(user->holdLock) == 0);
}

void UserDrop(POnlineUser user)
{
    if (user && user->state == OUS_ONLINE)
    {
        pthread_rwlock_unlock(user->holdLock);
    }
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
        {
            continue;
        }
        for (int j = 0; j < group->friendCount; ++j)
        {
            if (group->friends[j] == user->uid)
            {
                continue;
            }
            duser = OnlineUserGet(group->friends[j]);
            if (duser)
            {
                if (duser->state == OUS_ONLINE)
                {
                    CRPFriendNotifySend(duser->crp, 0, type, user->uid, 0, 0);
                }
                UserDrop(duser);
            }
        }
    }
    pthread_rwlock_unlock(info->friendsLock);
}

int PendingUserDelete(PPendingUser user)
{
    if (user->state == OUS_ONLINE)
    {
        return OnlineUserDelete((POnlineUser) user);
    }
    else if (user->state == OUS_PENDING_CLEAN)
    {
        return 0;
    }
    UserSetState((POnlineUser) user, OUS_PENDING_CLEAN, 0);
    PendingUserTableRemove(user);
    pthread_rwlock_destroy(user->holdLock);
    free(user->holdLock);
    free(user);
    return 1;
}

int OnlineUserDelete(POnlineUser user)
{
    if (user->state == OUS_PENDING_HELLO || user->state == OUS_PENDING_LOGIN)
    {
        return PendingUserDelete((PPendingUser) user);
    }
    if (user->state == OUS_PENDING_CLEAN)
    {
        return 1;
    }
    if (user->state != OUS_ONLINE)
    {
        log_warning("UserManager", "Trying to delete online user on illegal user state.\n");
        return 0;
    }
    if (UserSetState(user, OUS_PENDING_CLEAN, 0) == NULL)
    {
        return 0;
    }
    pthread_rwlock_unlock(user->holdLock);

    pthread_rwlock_wrlock(user->holdLock);
    UserOperationRemoveAll(user);
    pthread_mutex_destroy(&user->operations.lock);
    CRPClose(user->crp);
    if (user->info)
    {
        log_info("UserManager", "User %d offline.\n", user->uid);
        UserInfo *info = UserInfoGet(user->uid);
        if (info)
        {
            time_t now;
            time(&now);
            info->lastlogout = now;
            info->level += (now - user->info->loginTime) / 60;
            UserInfoSave(info->uid, info);
            UserInfoDrop(info);
        }

        UserFriendsDrop(user->uid);

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

POnlineUser UserSetState(POnlineUser user, OnlineUserState state, uint32_t uid)
{
    if (user->state == OUS_PENDING_CLEAN)
    {
        //正在清理的用户内存区域可能正在释放.此时不应改变状态.
        return NULL;
    }
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
        JobManagerKick(user);   //清除当前用户待处理任务
        user = ret;

        user->uid = uid;

        char path[256];
        uint8_t userDirSize;
        UserGetDir(path, uid, "");
        userDirSize = (uint8_t) strlen(path);
        struct stat buf;
        if (stat(path, &buf) || !S_ISDIR(buf.st_mode))//如果无法找到用户目录则重建用户目录
        {
            UserCreateDirectory(uid);
        }
        //用户在线信息初始化:
        POnlineUserInfo info = (POnlineUserInfo) calloc(1, sizeof(OnlineUserInfo));
        if (info == NULL)//内存错误!用户当前既不在等待状态,也无法切换到在线用户状态
        {
            PendingUserDelete(pendingUser);
            return NULL;
        }
        info->userDir = (char *) malloc(userDirSize + 1);
        if (info->userDir == NULL)//内存错误!用户当前既不在等待状态,也无法切换到在线用户状态
        {
            free(info);
            PendingUserDelete(pendingUser);
            return NULL;
        }
        memcpy(info->userDir, path, userDirSize);
        info->userDir[userDirSize] = 0;

        info->friends = UserFriendsGet(uid, &info->friendsLock, -1);//绑定用户好友列表
        info->message = UserMessageFileGet(uid);                    //绑定消息记录
        user->info = info;
        bzero(&user->operations, sizeof(UserOperationTable));
        //初始化用户操作锁
        pthread_mutexattr_t recursiveAttr;
        pthread_mutexattr_init(&recursiveAttr);
        pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&user->operations.lock, &recursiveAttr);
        user->state = OUS_ONLINE;
        UserBroadcastNotify(user, FNT_FRIEND_ONLINE);           //向好友们广播上线消息
        return OnlineUserTableSet(user);
    }
    else if (user->state == OUS_ONLINE && state == OUS_PENDING_CLEAN)
    {  //在线转待清理.(代码比较多.info对象销毁部分在OnlineUserDelete函数中)
        OnlineUserTableRemove(user->uid);//从在线用户表中移除当前用户
        EpollRemove(user);              //从epoll中移除当前用户
        JobManagerKick(user);           //删除待处理事务
        if (user->status != UOS_HIDDEN) //如果用户当前状态不是隐身状态则广播用户离线
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

void PostMessage(UserMessage *message)
{
    {
        MessageFile *file = UserMessageFileGet(message->to);
        if (file)
        {
            MessageFileInsert(file, message);
            UserMessageFileDrop(message->to);
        }
        file = UserMessageFileGet(message->from);
        if (file)
        {
            MessageFileInsert(file, message);
            UserMessageFileDrop(message->from);
        }
    }
    POnlineUser toUser = OnlineUserGet(message->to);
    if (toUser != NULL)
    {
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
        log_warning("PostMessage", "User Offline.\n");
    }
}