#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <logger.h>
#include <user.h>
#include <packets.h>
#include <unistd.h>
#include <sys/stat.h>
#include <data/user.h>

pthread_mutex_t UsersTableLock = PTHREAD_MUTEX_INITIALIZER;
UsersTable OnlineUsers = {
        .count=0,
        .first=NULL,
        .last=NULL
};

int(*PacketsProcessMap[CRP_PACKET_ID_MAX + 1])(OnlineUser *user, void *packet) = {
        [CRP_PACKET_KEEP_ALIVE]         =(int (*)(OnlineUser *user, void *packet)) ProcessPacketStatusKeepAlive,
        [CRP_PACKET_HELLO]              =(int (*)(OnlineUser *user, void *packet)) ProcessPacketStatusHello,
        [CRP_PACKET_OK]                 =(int (*)(OnlineUser *user, void *packet)) ProcessPacketStatusOK,
        [CRP_PACKET_FAILURE]            =(int (*)(OnlineUser *user, void *packet)) ProcessPacketStatusFailure,
        [CRP_PACKET_CRASH]              =(int (*)(OnlineUser *user, void *packet)) ProcessPacketStatusCrash,

        [CRP_PACKET_LOGIN__START]       =(int (*)(OnlineUser *user, void *packet)) NULL,
        [CRP_PACKET_LOGIN_LOGIN]        =(int (*)(OnlineUser *user, void *packet)) ProcessPacketLoginLogin,
        [CRP_PACKET_LOGIN_LOGOUT]       =(int (*)(OnlineUser *user, void *packet)) ProcessPacketLoginLogout,


        [CRP_PACKET_INFO__START]        = (int (*)(OnlineUser *user, void *packet)) NULL,
        [CRP_PACKET_INFO_QUERY]          = (int (*)(OnlineUser *user, void *packet)) ProcessPacketInfoQuery,
        [CRP_PACKET_INFO_DATA]        = (int (*)(OnlineUser *user, void *packet)) NULL,

        [CRP_PACKET_MESSAGE__START]     =(int (*)(OnlineUser *user, void *packet)) NULL,
        [CRP_PACKET_MESSAGE_TEXT]       =(int (*)(OnlineUser *user, void *packet)) ProcessPacketMessageTextMessage,
};

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

OnlineUser *OnlineUserNew(int fd)
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

void OnlineUserDelete(OnlineUser *user)
{
    shutdown(user->sockfd, SHUT_RDWR);
    close(user->sockfd);
    free(user->info);
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


UserOnlineInfo *UserCreateOnlineInfo(OnlineUser *user, uint32_t uid)
{
    char userDir[20];
    UserGetDir(userDir, uid, "");
    struct stat buf;
    if (stat(userDir, &buf) || !S_ISDIR(buf.st_mode))
    {
        UserCreateDirectory(uid);
    }
    UserOnlineInfo *info = (UserOnlineInfo *) malloc(sizeof(OnlineUser));
    memset(info, 0, sizeof(UserOnlineInfo));
    info->uid = uid;

    return info;
}