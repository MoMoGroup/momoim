#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <logger.h>
#include <user.h>
#include <packets.h>

pthread_mutex_t UsersTableLock = {0};
UsersTable OnlineUsers = {
        .count=0,
        .first=NULL,
        .last=NULL
};

OnlineUser *createUser(int fd)
{
    OnlineUser *user = (OnlineUser *) malloc(sizeof(OnlineUser));
    memset(user, 0, sizeof(OnlineUser));
    user->fd = fd;
    pthread_mutex_init(&user->writeLock, NULL);

    pthread_mutex_lock(&UsersTableLock);
    user->prev = OnlineUsers.last;

    if (OnlineUsers.last)
    {
        OnlineUsers.last->next = user;
    }
    else
    {
        OnlineUsers.first = user;
    }

    OnlineUsers.last = user;
    OnlineUsers.count++;
    pthread_mutex_unlock(&UsersTableLock);

    return user;
}

void deleteUser(OnlineUser *user)
{
    pthread_mutex_lock(&UsersTableLock);
    if (OnlineUsers.first == user)
    {
        OnlineUsers.first = user->next;
    }
    if (OnlineUsers.last == user)
    {
        OnlineUsers.last = user->prev;
    }
    if (user->prev)
        user->prev->next = user->next;
    pthread_mutex_unlock(&UsersTableLock);
    free(user);
}

/**
* Server Process User Message
*/
int processUser(OnlineUser *user, CRPBaseHeader *packet)
{
    int ret = 1;
    void *(*packetCast)(CRPBaseHeader *base) = PacketsDataCastMap[packet->packetID];
    if (packetCast == NULL)
    {
        return 0;
    }
    int(*packetProcessor)(OnlineUser *user, void *packet) = PacketsProcessMap[packet->packetID];
    if (packetProcessor != NULL)
    {
        void *data = packetCast(packet);
        ret = packetProcessor(user, data);
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

    switch (packet->packetID)
    {
        case CRP_PACKET_HELLO:
        {
        }
            break;
        case CRP_PACKET_LOGIN_LOGOUT:
        {
            if (user->uid != UINT32_MAX)
            {
                log_info("UserProc", "(ID:%4d)Logged out.\n", user->uid);
                return 0;
            }
        }
            break;
        case CRP_PACKET_LOGIN_LOGIN:
        {

        };
    }
    return 1;
}
