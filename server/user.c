#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <logger.h>
#include <user.h>

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

    switch (packet->packetID)
    {
        case CRP_PACKET_HELLO:
        {
            if (user->status == OUS_PENDING_HELLO)
            {
                CRPPacketHello *message = (CRPPacketHello *) packet->data;
                if (message->protocolVersion != 1)
                {
                    //log_info("UserProc", "Try to use wrong protocol to login.\n");
                    CRPLoginLoginFailureSend(user->fd, "Wrong protocol version.");
                    break;
                }
                log_info("UserProc", "Hello.\n");
                CRPOKSend(user->fd);
                user->status = OUS_PENDING_LOGIN;
            }
            else
            {

            }
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
