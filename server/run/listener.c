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
#include <fcntl.h>

#include <run/user.h>
#include <run/jobs.h>

static int ServerIOPool;

void UserJoinToPool(POnlineUser user)
{
    struct epoll_event event = {
            .data.ptr=user,
            .events=EPOLLERR | EPOLLIN
    };
    if (-1 == epoll_ctl(ServerIOPool, EPOLL_CTL_ADD, user->sockfd, &event))
    {
        perror("epoll_add");
    }
}

void UserRemoveFromPool(POnlineUser user)
{
    if (-1 == epoll_ctl(ServerIOPool, EPOLL_CTL_DEL, user->sockfd, NULL))
    {
        if (errno != ENOENT)
            perror("epoll_remove");
    }
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

    if (-1 == listen(sockfd, LISTENER_BACKLOG))
    {
        perror("listen");
        return NULL;
    }
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl");
        return NULL;
    }

    flags |= O_NONBLOCK;
    flags = fcntl(sockfd, F_SETFL, flags);
    if (flags == -1)
    {
        perror("fcntl");
        return NULL;
    }
    log_info("SERVER-MAIN", "Listenning on TCP %d\n", LISTEN_PORT);

    ServerIOPool = epoll_create1(EPOLL_CLOEXEC);

    struct epoll_event event = {
            .data.ptr=NULL,
            .events=EPOLLET | EPOLLIN
    };
    epoll_ctl(ServerIOPool, EPOLL_CTL_ADD, sockfd, &event); //将监听socket加入epoll(data.ptr==NULL)

    struct epoll_event *events = calloc(EPOLL_BACKLOG, sizeof event);
    while (IsServerRunning)
    {
        int n = epoll_wait(ServerIOPool, events, EPOLL_BACKLOG, -1);
        for (int i = 0; i < n; i++)
        {
            if (events[i].data.ptr == NULL)
            {
                int fd;
                socklen_t addr_len = sizeof addr;
                while (1)
                {
                    fd = accept(sockfd, (struct sockaddr *) &addr, &addr_len);
                    if (-1 == fd)
                    {
                        if (errno == EWOULDBLOCK)
                        {
                            break;
                        }
                        else
                        {
                            log_error("SERVER-LISTENER", "accept failure:%s\n", strerror(errno));
                            break;
                        }
                    }
                    flags = fcntl(fd, F_GETFL, 0);
                    flags |= O_NONBLOCK;
                    flags = fcntl(fd, F_SETFL, flags);
                    if (flags == -1)
                    {
                        perror("fcntl");
                        continue;
                    }
                    POnlineUser user = OnlineUserNew(fd);
                    UserJoinToPool(user);
                }
            }
            else
            {
                UserRemoveFromPool(events[i].data.ptr);
                JobManagerPush((POnlineUser) events[i].data.ptr);
            }
        }

    }
    free(events);
    close(ServerIOPool);
    return (void *) -1;
}