#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <logger.h>
#include <worker.h>
#include <user.h>
#include <jobs.h>
#include <arpa/inet.h>

int ServerIOPoll;

WorkerType worker[WORKER_COUNT];

void initWorker(int id, WorkerType *worker)
{
    worker->workerId = id;
    sem_init(&worker->ready, 0, 0);
    pthread_mutex_init(&worker->lock, NULL);
    pthread_create(&worker->WorkerThread, NULL, WorkerMain, worker);
}

void UserJoinToPoll(OnlineUser *user)
{
    struct epoll_event event = {
            .data.ptr=user,
            .events=EPOLLERR | EPOLLIN
    };
    epoll_ctl(ServerIOPoll, EPOLL_CTL_ADD, user->fd, &event);
}

void UserRemoveFromPoll(OnlineUser *user)
{
    epoll_ctl(ServerIOPoll, EPOLL_CTL_DEL, user->fd, NULL);

}

void *ListenMain(void *listenSocket)
{
    log_info("SERVER-LISTENER", "Preparing to accept connection..\n");
    int sockfd = *(int *) listenSocket;

    InitJobManger();

    int i;
    for (i = 0; i < WORKER_COUNT; i++)
    {
        initWorker(i, worker + i);
    }

    ServerIOPoll = epoll_create1(0);

    struct epoll_event event = {
            .data.ptr=0,
            .events=EPOLLET | EPOLLIN
    };
    epoll_ctl(ServerIOPoll, EPOLL_CTL_ADD, sockfd, &event);

    struct epoll_event *events = calloc(64, sizeof event);
    while (!server_exit)
    {
        int n = epoll_wait(ServerIOPoll, events, 64, -1);
        for (i = 0; i < n; i++)
        {
            if (events[i].data.ptr == 0)
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
                log_info("SERVER-LISTENER", "[%s:%d]Assigned to worker %d.\n",
                         inet_ntoa(addr.sin_addr),
                         ntohs(addr.sin_port),
                         worker->workerId
                );
                UserJoinToPoll(createUser(fd));
            }
            else
            {
                UserRemoveFromPoll(events[i].data.ptr);
                PushJob(events[i].data.ptr);
            }
        }

    }

    return (void *) -1;
}