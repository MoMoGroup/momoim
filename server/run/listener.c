#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <logger.h>

#include <run/user.h>
#include <run/jobs.h>
#include <run/natTraversal.h>

static int ServerIOPool;

void EpollAdd(POnlineUser user)
{
    struct epoll_event event = {
            .data.ptr=user,
            .events=EPOLLERR | EPOLLIN
    };
    if (-1 == epoll_ctl(ServerIOPool, EPOLL_CTL_ADD, user->crp->fd, &event)) //用户无法被添加到epoll中
    {
        perror("epoll_add");
    }
}

void EpollModify(POnlineUser user)
{
    struct epoll_event event = {
            .data.ptr=user,
            .events=EPOLLERR | EPOLLIN
    };
    if (-1 == epoll_ctl(ServerIOPool, EPOLL_CTL_MOD, user->crp->fd, &event)) //用户无法被添加到epoll中
    {
        perror("epoll modify");
    }
}

void EpollRemove(POnlineUser user)
{
    if (-1 == epoll_ctl(ServerIOPool, EPOLL_CTL_DEL, user->crp->fd, NULL))
    {
        if (errno != ENOENT)
        {//如果fd已经移除,不要报错
            perror("epoll_remove");
        }
    }
}

static void listenLoop(int sockListener, int sockIdx, struct epoll_event *events)
{
    char keyBuffer[32];
    struct sockaddr_in idxSock;
    struct sockaddr_in addr;
    socklen_t addrLen;
    while (IsServerRunning)
    {
        int n = epoll_wait(ServerIOPool, events, CONFIG_EPOLL_QUEUE, -1);
        if (n == -1)
        {
            if (errno != EINTR)
            {
                log_warning("Listener", "epoll_wait:%s.\n", strerror(errno));
            }
            continue;
        }
        for (int i = 0; i < n; i++)
        {
            if (events[i].data.ptr == NULL) //TCP客户端连接
            {
                int fd;
                socklen_t addr_len = sizeof addr;
                while (1)
                {
                    fd = accept(sockListener, (struct sockaddr *) &addr, &addr_len);  //尝试接受一个客户端
                    if (-1 == fd)
                    {

                        if (errno == EWOULDBLOCK)   //如果已经没有客户端可Accept了
                        {
                            break;//退出循环,执行下一个事件
                        }
                        else
                        {
                            log_error("SERVER-LISTENER",
                                      "accept failure:%s\n",
                                      strerror(errno));
                            break;
                        }
                    }
                    if (OnlineUserCount >= CONFIG_MAX_CLIENTS)
                    {
                        close(fd);
                        continue;
                    }
                    fcntl(fd, F_SETFL, O_NONBLOCK);
                    PPendingUser user = UserNew(fd);//分配一个用户对象空间(只做简单初始化)
                    EpollAdd((POnlineUser) user);//将其加入事务池
                }
            }
            else if (events[i].data.ptr == (void *) -1) //NAT发现
            {
                addrLen = sizeof(idxSock);
                if (32 == recvfrom(sockIdx,
                                   keyBuffer,
                                   sizeof(keyBuffer),
                                   0,
                                   (struct sockaddr *) &idxSock,
                                   &addrLen))
                {
                    NatHostDiscoverNotify(&idxSock, keyBuffer);
                }
            }
            else
            {
                EpollRemove(events[i].data.ptr); //用户数据到达
                JobManagerPush((POnlineUser) events[i].data.ptr);//将用户加入到事务管理器(可能会阻塞)
            }
        }

    }
}

static int prepareListener()
{
    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(CONFIG_LISTEN_PORT),
            .sin_addr.s_addr = htons(INADDR_ANY)
    };

    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1)
    {
        close(fd);
        return -1;
    }
    int on = 1;
    if ((setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        perror("setsockopt reuse address");
        close(fd);
        return -1;
    }

    if (-1 == bind(fd, (struct sockaddr *) &addr, sizeof addr))
    {
        perror("bind");
        close(fd);
        return -1;
    }

    if (-1 == listen(fd, CONFIG_LISTENER_BACKLOG))
    {
        perror("listen");
        close(fd);
        return -1;
    }
    {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1)
        {
            perror("fcntl");
            close(fd);
            return -1;
        }

        flags |= O_NONBLOCK;
        flags = fcntl(fd, F_SETFL, flags);
        if (flags == -1)
        {
            perror("fcntl");
            close(fd);
            return -1;
        }
    }
    return fd;
}

static int prepareIndexer()
{
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int on = 1;
    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(CONFIG_HOST_DISCOVER_PORT),
            .sin_addr.s_addr = htons(INADDR_ANY)
    };

    if (fd == -1)
    {
        close(fd);
        perror("socket");
        return -1;
    }

    if ((setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        perror("setsockopt reuse address");
        close(fd);
        return -1;
    }

    if (-1 == bind(fd, (struct sockaddr *) &addr, sizeof addr))
    {
        perror("bind");
        close(fd);
        return -1;
    }
    return fd;

}

void *ListenMain(void *nouse)
{
    int sockListener, sockIdx;
    sockListener = prepareListener();
    if (sockListener < 0)
    {
        return NULL;
    }
    pthread_cleanup_push(close, sockListener);
            sockIdx = prepareIndexer();
            if (sockIdx < 0)
            {
                break;//跳出本层pthread_cleanup_push,会执行cleanup_pop
            }
            pthread_cleanup_push(close, sockIdx);

                    log_info("Listener", "主服务正在监听TCP %d\n", CONFIG_LISTEN_PORT);
                    log_info("Listener", "NAT穿透服务正在等待UDP %d.\n", CONFIG_HOST_DISCOVER_PORT);

                    ServerIOPool = epoll_create1(EPOLL_CLOEXEC);
                    {
                        struct epoll_event event = {
                                .data.ptr=NULL,
                                .events=EPOLLET | EPOLLIN                               //EPOLLET-边沿触发. EPOLLIN-数据传入时触发
                        };
                        epoll_ctl(ServerIOPool,
                                  EPOLL_CTL_ADD,
                                  sockListener,
                                  &event);   //将监听socket加入epoll(data.ptr==NULL)

                        event.data.ptr = (void *) -1;
                        event.events = EPOLLERR | EPOLLIN | EPOLLET;
                        epoll_ctl(ServerIOPool, EPOLL_CTL_ADD, sockIdx, &event);        //NAT发现索引SOCKET
                    }
                    struct epoll_event *events = calloc(CONFIG_EPOLL_QUEUE, sizeof(struct epoll_event));
                    pthread_cleanup_push(free, events);
                            listenLoop(sockListener, sockIdx, events);
                    pthread_cleanup_pop(1);
            pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    return (void *) -1;
}