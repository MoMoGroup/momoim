#pragma once

#include <sys/socket.h>
#include <pthread.h>
#include <aio.h>
#include <protocol.h>
#include <imcommon/user.h>

#include "Structures.h"
#include "run/worker.h"
#include "packets.h"

//在线用户状态
typedef enum
{
    OUS_PENDING_HELLO = 0,
    OUS_PENDING_LOGIN,


    OUS_ONLINE = 0x10,
    OUS_PENDING_CLEAN
} OnlineUserState;
typedef enum
{
    CUOT_FILE_REQUEST,
    CUOT_FILE_STORE,
    CUOT_NAT_DISCOVER
} UserOperationType;

//消息句柄
struct structUserMessageProcessor
{
    packet_id_t packetID;

    PacketHandler handler;

    PUserMessageProcessor prev;
    PUserMessageProcessor next;
    void *data;
};

//存储文件操作
struct structUserOperationFileStore
{
    size_t totalLength, remainLength, seq;
    unsigned char key[16];
    char tmpfile[30];
    int fd;
};
//请求文件操作
struct structUserOperationFileRequest
{
    struct aiocb aio;
    off_t size;
    size_t seq;
};
//用户操作
struct structUserOperation
{
    uint32_t session;
    int type;
    int cancel;
    void *data;

    OperationEventHandler onCancel, onResponseOK, onResponseFailure;
    pthread_mutex_t lock;

    PUserOperation prev;
    PUserOperation next;
};

//用户操作表
struct structUserOperationTable
{
    PUserOperation first;
    PUserOperation last;
    pthread_mutex_t lock;
    pthread_cond_t unlockCond;
    int count;
};

//用户在线信息
struct structOnlineUserInfo
{
    uint32_t uid;
    char *userDir;
    time_t loginTime;
    UserOnlineStatus status;
    UserFriends *friends;
    pthread_rwlock_t *friendsLock;
};

//在线用户数据
struct structOnlineUser
{
    //与待登陆用户数据保持一致
    CRPContext crp;
    volatile OnlineUserState state;
    pthread_rwlock_t *holdLock;
    time_t lastUpdateTime;

    //该状态私有数据
    POnlineUserInfo info;
    UserOperationTable operations;
};
//等待登陆用户数据
struct structPendingUser
{
    CRPContext sockfd;
    volatile OnlineUserState state;
    pthread_rwlock_t *holdLock;
    time_t lastUpdateTime;

    //待登陆私有数据Online
    struct structPendingUser *prev, *next;
};

//在线用户表
struct structOnlineUsersTableType
{
    OnlineUser *user;
    struct structOnlineUsersTableType *prev, *next[0x10];
};
//待登陆用户表
struct structPendingUsersTableType
{
    PendingUser *first, *last;
};


//处理用户消息
int ProcessUser(POnlineUser user, CRPBaseHeader *packet);

//投递消息
void PostMessage(UserMessage *message);

void UserBroadcastNotify(POnlineUser user, FriendNotifyType type);

//保持用户.
int UserHold(POnlineUser user);

//释放用户.
void UserDrop(POnlineUser user);

//创建一个等待用户对象
PPendingUser PendingUserNew(int fd);

int PendingUserDelete(PPendingUser);

//删除一个在线用户对象,如果用户被保持,它将阻塞当前线程,直到用户被释放.
int OnlineUserDelete(POnlineUser user);

//设置用户状态
//该函数作用包括切换用户状态,更改存储位置,初始化状态参数.删除状态参数,状态更变通知
POnlineUser UserSetStatus(POnlineUser user, OnlineUserState state, uint32_t uid);

//通过uid查找用户
POnlineUser OnlineUserGet(uint32_t uid);

//注册一个可取消用户操作
PUserOperation UserOperationRegister(POnlineUser user, session_id_t sessionID, int type, void *data) __attribute_malloc__;

//注销一个可取消用户操作.如果operation是孤立的(无前后节点),它将被简单的释放.
void UserOperationUnregister(POnlineUser user, PUserOperation op);

//通过操作ID获得用户操作
PUserOperation UserOperationGet(POnlineUser user, uint32_t sessionId);

void UserOperationDrop(POnlineUser user, PUserOperation);

//通过自定义函数和操作获得用户操作
//type为-1时会枚举所有操作.
//注意,不要试图在枚举时注册或注销一个操作
PUserOperation UserOperationQuery(POnlineUser user, UserOperationType type, int (*func)(PUserOperation op, void *data), void *data);

//取消一个操作.
//如果操作oncancel字段不为空,会调用oncancel指定的函数.
int UserOperationCancel(POnlineUser user, PUserOperation op);

//取消并清空一个用户的所有操作.所有已存在操作将被取消并解除与用户操作链表的关联.
//注意,该函数并不会释放相应内存空间,需要操作注册线程调用注销函数来释放这块内存.
void UserOperationRemoveAll(POnlineUser user);

void InitUserManager();

void FinalizeUserManager();

//在线的小伙伴们！！
extern OnlineUsersTableType OnlineUserTable;