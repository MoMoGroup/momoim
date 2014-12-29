#pragma once

#include <sys/socket.h>
#include <pthread.h>
#include <worker.h>
#include <protocol.h>

//在线用户状态
typedef enum
{
    OUS_PENDING_HELLO = 0,
    OUS_PENDING_LOGIN = 1,


    OUS_ONLINE = 0x10
} OnlineUserStatus;

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
    size_t totalLength, remainLength;
    unsigned char key[16];
    char *tmpfile;
    int fd;
} UserFileStoreOperation;
//可取消操作
typedef struct struUserCancelableOperation
{
    uint32_t id;
    int cancel;
    void *data;

    int (*oncancel)(struct struOnlineUser *, struct struUserCancelableOperation *);

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
    OnlineUserStatus status;
    OnlineUserInfo *info;


    UserCancelableOperationTable operations;
    pthread_mutex_t sockLock;
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

//删除一个在线用户对象
void OnlineUserDelete(OnlineUser *user);

OnlineUserInfo *UserCreateOnlineInfo(OnlineUser *user, uint32_t uid);

__attribute_malloc__ UserCancelableOperation *UserRegisterOperation(OnlineUser *user);

void UserUnregisterOperation(OnlineUser *user, UserCancelableOperation *operation);

int UserCancelOperation(OnlineUser *user, uint32_t operationId);

//在线的小伙伴们！！
extern UsersTable OnlineUsers;
//用户表写锁
extern pthread_rwlock_t UsersTableLock;