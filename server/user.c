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
    OnlineUser *user = (OnlineUser *) calloc(1, sizeof(OnlineUser));

    user->sockfd = fd;
    pthread_mutex_init(&user->writeLock, NULL);
    user->status = OUS_PENDING_HELLO;

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
    void *data;
    void *(*packetCast)(CRPBaseHeader *) = PacketsDataCastMap[packet->packetID];
    if (packetCast == NULL)
    {
        return 0;
    }
    data = packetCast(packet);
    if (data == NULL)
        return 0;

    int(*packetProcessor)(OnlineUser *, void *) = PacketsProcessMap[packet->packetID];
    if (packetProcessor != NULL)
    {
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
}
