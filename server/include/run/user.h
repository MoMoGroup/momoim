#pragma once

#include <sys/socket.h>
#include <pthread.h>
#include <protocol.h>
#include "run/worker.h"

//在线用户状态
typedef enum
{
    OUS_PENDING_INIT = 0, //PendingInit - Must be zero
    OUS_PENDING_HELLO,
    OUS_PENDING_LOGIN,


    OUS_ONLINE = 0x10,
    OUS_PENDING_CLEAN,
    OUS_CLEANED
} OnlineUserStatus;
typedef enum
{
    CUOT_FILE_SEND,
    CUOT_FILE_STORE
} UserCancelableOperationType;
struct struOnlineUserInfo;
struct struUserFileStoreOperation;
struct struUserCancelableOperation;
struct struUserCancelableOperationTable;
struct struOnlineUser;

//在线用户基本信息
typedef struct struOnlineUserInfo
{
    uint32_t uid;
    char *userDir;
} OnlineUserInfo;

typedef struct struUserFileStoreOperation
{
    uint32_t session;
    size_t totalLength, remainLength, seq;
    unsigned char key[16];
    char tmpfile[30];
    int fd;
    pthread_mutex_t lock;
} UserFileStoreOperation;
//可取消操作
typedef struct struUserCancelableOperation
{
    uint32_t id;
    int type;
    int cancel;
    void *data;

    int (*onCancel)(struct struOnlineUser *, struct struUserCancelableOperation *);

    struct struUserCancelableOperation *prev;
    struct struUserCancelableOperation *next;
} UserCancelableOperation;

//用户操作表
typedef struct struUserCancelableOperationTable
{
    UserCancelableOperation *first;
    UserCancelableOperation *last;
    pthread_rwlock_t lock;
    int count;
} UserCancelableOperationTable;

//在线用户数据
typedef struct struOnlineUser
{
    int sockfd;
    volatile OnlineUserStatus status;
    OnlineUserInfo *info;

    pthread_rwlock_t holdLock;

    UserCancelableOperationTable operations;
    struct struOnlineUser *prev;
    struct struOnlineUser *next;
} OnlineUser;

//在线用户链表
typedef struct
{
    OnlineUser *first;
    OnlineUser *last;
    int count;
} UsersTable;

//处理用户消息
int ProcessUser(OnlineUser *user, CRPBaseHeader *packet);

//创建一个在线用户对象
OnlineUser *OnlineUserNew(int fd);

void OnlineUserInit(OnlineUser *);

//删除一个在线用户对象,如果用户被保持,它将阻塞当前线程,直到用户被释放.
int OnlineUserDelete(OnlineUser *user);

//保持用户.
int OnlineUserHold(OnlineUser *user);

//释放用户.
void OnlineUserDrop(OnlineUser *user);

//设置用户状态
void OnlineUserSetStatus(OnlineUser *user, OnlineUserStatus status);

//通过uid查找用户
OnlineUser *OnlineUserGet(uint32_t uid);

//通过创建一个用户在线信息字段.
//该字段用于加速对在线用户数据的操作.
OnlineUserInfo *UserCreateOnlineInfo(OnlineUser *user, uint32_t uid) __attribute_malloc__;

//释放一个在线用户字段
void UserFreeOnlineInfo(OnlineUser *user);

//注册一个可取消用户操作
UserCancelableOperation *UserOperationRegister(OnlineUser *user, int type) __attribute_malloc__;

//注销一个可取消用户操作.如果operation是孤立的(无前后节点),它将被简单的释放.
void UserOperationUnregister(OnlineUser *user, UserCancelableOperation *op);

//通过操作ID获得用户操作
UserCancelableOperation *UserOperationGet(OnlineUser *user, uint32_t operationId);

//通过自定义函数和操作获得用户操作
//type为-1时会枚举所有操作.
//注意,不要试图在枚举时注册或注销一个操作
UserCancelableOperation *UserOperationQuery(OnlineUser *user, UserCancelableOperationType type, int (*func)(UserCancelableOperation *op, void *data), void *data);

//取消一个操作.
//如果操作oncancel字段不为空,会调用oncancel指定的函数.
int UserOperationCancel(OnlineUser *user, UserCancelableOperation *op);

//取消并清空一个用户的所有操作.所有已存在操作将被取消并解除与用户操作链表的关联.
//注意,该函数并不会释放相应内存空间,需要操作注册线程调用注销函数来释放这块内存.
void UserOperationRemoveAll(OnlineUser *user);

//在线的小伙伴们！！
extern UsersTable OnlineUserTable;
//用户表写锁
extern pthread_rwlock_t UsersTableLock;