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

static pthread_mutex_t banListLock = PTHREAD_MUTEX_INITIALIZER;
static in_addr_t *banList;
static size_t banCount;
static int serverIOPool;

void BanListAdd(in_addr_t ip)
{
    pthread_mutex_lock(&banListLock);
    void *ptr = realloc(banList, (banCount + 1) * sizeof(in_addr_t));
    if (ptr)
    {
        banList = ptr;
        banList[banCount++] = ip;
    }
    pthread_mutex_unlock(&banListLock);
}

void BanListRemove(in_addr_t ip)
{
    pthread_mutex_lock(&banListLock);
    for (size_t i = 0; i < banCount; ++i)
    {
        if (banList[i] == ip)
        {
            memcpy(banList + i, banList + i + 1, (banCount - i - 1) * sizeof(in_addr_t));
            void *ptr = realloc(banList, --banCount);
            if (ptr)
            {
                banList = ptr;
            }
            break;
        }
    }
    pthread_mutex_unlock(&banListLock);
}

void EpollAdd(POnlineUser user)
{
    struct epoll_event event = {
            .data.ptr=user,
            .events=EPOLLERR | EPOLLIN
    };
    if (-1 == epoll_ctl(serverIOPool, EPOLL_CTL_ADD, user->crp->fd, &event)) //用户无法被添加到epoll中
    {
        perror("epoll_ctl EPOLL_CTL_ADD");
    }
}

void EpollModify(POnlineUser user)
{
    struct epoll_event event = {
            .data.ptr=user,
            .events=EPOLLERR | EPOLLIN
    };
    if (-1 == epoll_ctl(serverIOPool, EPOLL_CTL_MOD, user->crp->fd, &event)) //用户无法被添加到epoll中
    {
        perror("epoll_ctl EPOLL_CTL_MOD");
    }
}

void EpollRemove(POnlineUser user)
{
    if (-1 == epoll_ctl(serverIOPool, EPOLL_CTL_DEL, user->crp->fd, NULL))
    {
        if (errno != ENOENT)
        {
            perror("epoll_ctl EPOLL_CTL_DEL");
        }
    }
}

static void listenLoop(int sockListener, int sockIdx, struct epoll_event *events)
{
    uint8_t keyBuffer[32];
    struct sockaddr_in idxSock;
    struct sockaddr_in addr;
    socklen_t addrLen;
    while (IsServerRunning)
    {
        int n = epoll_wait(serverIOPool, events, CONFIG_EPOLL_QUEUE, -1);
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
                    nextAccept:
                    fd = accept(sockListener, (struct sockaddr *) &addr, &addr_len);  //尝试接受一个客户端
                    if (-1 == fd)
                    {

                        if (errno == EWOULDBLOCK || errno == EAGAIN)   //如果已经没有客户端可Accept了
                        {
                            break;//退出循环,执行下一个事件
                        }
                        else
                        {
                            log_error("Listener",
                                      "accept failure:%s\n",
                                      strerror(errno));
                            break;
                        }
                    }

                    pthread_mutex_lock(&banListLock);
                    //处理禁止访问IP表
                    for (size_t i = 0; i < banCount; ++i)
                    {
                        if (banList[i] == addr.sin_addr.s_addr)
                        {
                            close(fd);
                            pthread_mutex_unlock(&banListLock);
                            goto nextAccept;
                        }
                    }
                    pthread_mutex_unlock(&banListLock);
                    if (OnlineUserCount >= CONFIG_MAX_CLIENTS)
                    {
                        close(fd);
                        continue;
                    }
                    //fcntl(fd, F_SETFL, O_NONBLOCK);
                    PPendingUser user = UserNew(fd);//分配一个用户对象空间(只做简单初始化)
                    EpollAdd((POnlineUser) user);//将其加入事务池
                }
            }
            else if (events[i].data.ptr == (void *) -1) //NAT发现
            {
                addrLen = sizeof(idxSock);
                //尝试接收32字节密钥
                if (32 == recvfrom(sockIdx, keyBuffer, sizeof(keyBuffer), 0, (struct sockaddr *) &idxSock, &addrLen))
                {
                    //尝试发现
                    if (NatHostDiscoverNotify(&idxSock, keyBuffer))
                    {
                        //发现成功,回复发现密钥
                        sendto(sockIdx, keyBuffer, 32, 0, (struct sockaddr *) &idxSock, addrLen);
                    }
                    else
                    {
                        //发现失败,回复32字节0
                        sendto(sockIdx, (uint8_t[32]) {0,}, 32, 0, (struct sockaddr *) &idxSock, addrLen);
                    }
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
        IsServerRunning = 0;
        return NULL;
    }
    pthread_cleanup_push(close, sockListener);
            sockIdx = prepareIndexer();
            if (sockIdx < 0)
            {
                break;//跳出本层pthread_cleanup_push,会执行cleanup_pop
            }

            pthread_cleanup_push(close, sockIdx);
                    NatHostInitlize();

                    pthread_cleanup_push(NatHostFinalize, 0);
                            log_info("Listener", "Server is listener on TCP %d\n", CONFIG_LISTEN_PORT);
                            log_info("Listener", "NAT Traversal is waiting on UDP %d.\n", CONFIG_HOST_DISCOVER_PORT);

                            serverIOPool = epoll_create1(EPOLL_CLOEXEC);
                            {
                                struct epoll_event event = {
                                        .data.ptr=NULL,
                                        .events=EPOLLET | EPOLLIN                       //EPOLLET-边沿触发. EPOLLIN-数据传入时触发
                                };
                                epoll_ctl(serverIOPool,
                                          EPOLL_CTL_ADD,
                                          sockListener,
                                          &event);   //将监听socket加入epoll(data.ptr==NULL)

                                event.data.ptr = (void *) -1;
                                event.events = EPOLLERR | EPOLLIN | EPOLLET;
                                epoll_ctl(serverIOPool, EPOLL_CTL_ADD, sockIdx, &event);//NAT发现索引SOCKET
                            }
                            struct epoll_event *events = calloc(CONFIG_EPOLL_QUEUE, sizeof(struct epoll_event));
                            pthread_cleanup_push(free, events);
                                    listenLoop(sockListener, sockIdx, events);
                            pthread_cleanup_pop(1);
                    pthread_cleanup_pop(1);
            pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    IsServerRunning = 0;
    return (void *) -1;
}