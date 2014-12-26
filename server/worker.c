#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>

#include <worker.h>
#include <user.h>
#include <jobs.h>
#include <logger.h>


void *WorkerMain(void *arg)
{
    char workerName[10];
    WorkerType *worker = (WorkerType *) arg;

    sprintf(workerName, "WORKER-%d", worker->workerId);

    ssize_t ret;

    while (!server_exit)
    {
        OnlineUser *user = PollJob();

        CRPBaseHeader *header;
        header = CRPRecv(user->sockfd);
        if (header == NULL)
        {
            log_warning("WorkerMain", "Protocol Fail. Killing user.\n");
            free(header);
            epoll_ctl(ServerIOPoll, EPOLL_CTL_DEL, user->sockfd, NULL);
            shutdown(user->sockfd, SHUT_RDWR);
            close(user->sockfd);
            deleteUser(user);
            continue;
        }
        if (processUser(user, header) == 0)
        {
            free(header);
            shutdown(user->sockfd, SHUT_RDWR);
            close(user->sockfd);
            deleteUser(user);
            continue;
        }
        free(header);
        UserJoinToPoll(user);
    }
    return 0;
}