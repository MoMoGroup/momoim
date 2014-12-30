#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <logger.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <run/worker.h>
#include <run/user.h>
#include <run/jobs.h>

#define LISTEN_PORT 8014
int ServerIOPoll;

void UserJoinToPoll(OnlineUser *user)
{
    struct epoll_event event = {
            .data.ptr=user,
            .events=EPOLLERR | EPOLLIN
    };
    epoll_ctl(ServerIOPoll, EPOLL_CTL_ADD, user->sockfd, &event);
}

void UserRemoveFromPoll(OnlineUser *user)
{
    epoll_ctl(ServerIOPoll, EPOLL_CTL_DEL, user->sockfd, NULL);

}

void *ListenMain(void *listenSocket)
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1)
    {
        perror("socket");
        return NULL;
    }
    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(LISTEN_PORT),
            .sin_addr.s_addr = htons(INADDR_ANY)
    };

    int on = 1;
    if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        perror("setsockopt reuse address");
        close(sockfd);
        return NULL;
    }
    if (-1 == bind(sockfd, (struct sockaddr *) &addr, sizeof addr))
    {
        perror("bind");
        return NULL;
    }

    if (-1 == listen(sockfd, 50))
    {
        perror("listen");
        return NULL;
    }
    log_info("SERVER-MAIN", "Listenning on TCP %d\n", LISTEN_PORT);

    ServerIOPoll = epoll_create1(0);

    struct epoll_event event = {
            .data.ptr=NULL,
            .events=EPOLLET | EPOLLIN
    };
    epoll_ctl(ServerIOPoll, EPOLL_CTL_ADD, sockfd, &event); //将监听socket加入epoll(data.ptr==NULL)

    struct epoll_event *events = calloc(64, sizeof event);
    while (!server_exit)
    {
        int n = epoll_wait(ServerIOPoll, events, 64, -1);
        for (int i = 0; i < n; i++)
        {
            if (events[i].data.ptr == NULL)
            {
                int fd;
                struct sockaddr_in addr;
                socklen_t addr_len = sizeof addr;
                fd = accept(sockfd, (struct sockaddr *) &addr, &addr_len);
                if (-1 == fd)
                {
                    log_error("SERVER-LISTENER", "accept failure:%s\n", strerror(errno));
                    continue;
                }
                UserJoinToPoll(OnlineUserNew(fd));
            }
            else
            {
                UserRemoveFromPoll(events[i].data.ptr);
                PushJob(events[i].data.ptr);
            }
        }

    }
    free(events);

    return (void *) -1;
}