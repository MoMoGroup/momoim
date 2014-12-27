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

typedef struct struOnlineUserInfo
{
    uint32_t uid;

} UserOnlineInfo;

typedef struct __attribute__ ((packed))
{
    uint32_t uid;
    char nickName[32];
    char sex;
} UserInfo;

//在线用户数据
typedef
struct struOnlineUser
{
    int sockfd;
    UserOnlineInfo *info;
    OnlineUserStatus status;

    pthread_mutex_t writeLock;
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

//创建一个在线用户对象
OnlineUser *OnlineUserNew(int fd);

//处理用户消息
int processUser(OnlineUser *user, CRPBaseHeader *packet);

//删除一个在线用户对象
void OnlineUserDelete(OnlineUser *user);

UserInfo *UserGetInfo(uint32_t uid);

void UserGetDir(char *path, uint32_t uid, const char *relPath);

void UserCreateDirectory(uint32_t uid);

void UserCreateInfoFile(uint32_t uid, char *path);

UserOnlineInfo *UserCreateOnlineInfo(OnlineUser *user, uint32_t uid);

//在线的小伙伴们！！
extern UsersTable OnlineUsers;
//用户表写锁
extern pthread_mutex_t UsersTableLock;