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

void ServerListenerShutdown()
{
    close(ServerIOPool);
}

void EpollAdd(POnlineUser user)
{
    struct epoll_event event = {
            .data.ptr=user,
            .events=EPOLLERR | EPOLLIN
    };
    if (-1 == epoll_ctl(ServerIOPool, EPOLL_CTL_ADD, user->sockfd->fd, &event)) //用户无法被添加到epoll中
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
    if (-1 == epoll_ctl(ServerIOPool, EPOLL_CTL_MOD, user->sockfd->fd, &event)) //用户无法被添加到epoll中
    {
        perror("epoll modify");
    }
}

void EpollRemove(POnlineUser user)
{
    if (-1 == epoll_ctl(ServerIOPool, EPOLL_CTL_DEL, user->sockfd->fd, NULL))
    {
        if (errno != ENOENT)//如果fd已经移除,不要报错
            perror("epoll_remove");
    }
}

void *ListenMain(void *listenSocket)
{
    int sockListener, sockP2P;
    sockListener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockP2P = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockListener == -1 || sockP2P == -1)
    {
        close(sockListener);
        close(sockP2P);
        perror("socket");
        return NULL;
    }
    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(LISTEN_PORT),
            .sin_addr.s_addr = htons(INADDR_ANY)
    };

    int on = 1;
    if ((setsockopt(sockListener, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        perror("setsockopt reuse address");
        close(sockListener);
        close(sockP2P);
        return NULL;
    }

    if ((setsockopt(sockP2P, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        perror("setsockopt reuse address");
        close(sockListener);
        close(sockP2P);
        return NULL;
    }

    if (-1 == bind(sockListener, (struct sockaddr *) &addr, sizeof addr))
    {
        perror("bind");
        close(sockListener);
        close(sockP2P);
        return NULL;
    }
    if (-1 == bind(sockP2P, (struct sockaddr *) &addr, sizeof addr))
    {
        perror("bind");
        close(sockListener);
        close(sockP2P);
        return NULL;
    }

    if (-1 == listen(sockListener, LISTENER_BACKLOG))
    {
        perror("listen");
        close(sockListener);
        close(sockP2P);
        return NULL;
    }
    {
        int flags = fcntl(sockListener, F_GETFL, 0);
        if (flags == -1)
        {
            perror("fcntl");
            close(sockListener);
            close(sockP2P);
            return NULL;
        }

        flags |= O_NONBLOCK;
        flags = fcntl(sockListener, F_SETFL, flags);
        if (flags == -1)
        {
            perror("fcntl");
            close(sockListener);
            close(sockP2P);
            return NULL;
        }
    }
    log_info("Listener", "Listenning on TCP %d\n", LISTEN_PORT);
    log_info("Listener", "P2P Index Server Recving on UDP %d\n", LISTEN_PORT);

    ServerIOPool = epoll_create1(EPOLL_CLOEXEC);    //创建epoll,在服务端fork时关闭
    {
        struct epoll_event event = {
                .data.ptr=NULL,
                .events=EPOLLET | EPOLLIN               //EPOLLET-边沿触发.
        };
        epoll_ctl(ServerIOPool, EPOLL_CTL_ADD, sockListener, &event); //将监听socket加入epoll(data.ptr==NULL)

        event.data.ptr = (void *) -1;
        event.events = EPOLLERR | EPOLLIN | EPOLLET;
        epoll_ctl(ServerIOPool, EPOLL_CTL_ADD, sockP2P, &event);//P2P索引SOCKET
    }

    struct epoll_event *events = calloc(EPOLL_BACKLOG, sizeof(struct epoll_event));
    while (IsServerRunning)
    {
        int n = epoll_wait(ServerIOPool, events, EPOLL_BACKLOG, -1);
        if (n == -1)
        {
            log_warning("Listener", "epoll_wait failure.\n");
            break;
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
                        if (errno == EWOULDBLOCK)   //如果已经没有客户端可接受了
                        {
                            break;//退出循环,执行下一个事件
                        }
                        else
                        {
                            log_error("SERVER-LISTENER", "accept failure:%s\n", strerror(errno));
                            break;
                        }
                    }
                    fcntl(fd, F_SETFL, O_NONBLOCK);
                    PPendingUser user = PendingUserNew(fd);//分配一个用户对象空间(只做简单初始化)
                    EpollAdd((POnlineUser) user);//将其加入事务池
                }
            }
            else if (events[i].data.ptr == (void *) -1) //P2P发现
            {

                //recvfrom(sockP2P, <#(void*)__buf#>, <#(size_t)__n#>, <#(int)__flags#>, <#(struct sockaddr *)__addr#>, <#(socklen_t*)__addr_len#>)
            }
            else
            {
                EpollRemove(events[i].data.ptr); //用户数据到达
                JobManagerPush((POnlineUser) events[i].data.ptr);//将用户加入到事务管理器(可能会阻塞)
            }
        }

    }
    log_info("Listener", "Exiting...\n");
    free(events);
    close(ServerIOPool);
    return (void *) -1;
}