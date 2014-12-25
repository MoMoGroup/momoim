#pragma once

#include <sys/socket.h>
#include <pthread.h>
#include <worker.h>
#include <protocol.h>

enum OnlineUserStatus
{
    OUS_PENDING_HELLO = 0,
    OUS_PENDING_LOGIN = 1,


    OUS_ONLINE = 0x10
};
typedef
struct struOnlineUser
{
    int fd;
    unsigned int uid;
    enum OnlineUserStatus status;

    pthread_mutex_t writeLock;
    struct struOnlineUser *prev;
    struct struOnlineUser *next;
} OnlineUser;

typedef struct
{
    OnlineUser *first;
    OnlineUser *last;
    int count;
} UsersTable;

extern UsersTable OnlineUsers;
extern pthread_mutex_t UsersTableLock;

OnlineUser *createUser(int fd);

int processUser(OnlineUser *user, CRPBaseHeader *packet);

void deleteUser(OnlineUser *user);