#include <stdio.h>
#include <stdlib.h>

#include <worker.h>
#include <user.h>
#include <jobs.h>


void *WorkerMain(void *arg)
{
    char workerName[10];
    CRPBaseHeader *header;
    OnlineUser *user;
    WorkerType *worker = (WorkerType *) arg;

    sprintf(workerName, "WORKER-%d", worker->workerId);

    while (!server_exit)
    {
        user = PollJob();
        pthread_mutex_lock(&user->sockLock);
        header = CRPRecv(user->sockfd);     //由于CRPRecv会分两次recv接收数据,期间不可打断.所以必须加锁保护.
        pthread_mutex_unlock(&user->sockLock);
        UserJoinToPoll(user);

        if (header == NULL || processUser(user, header) == 0)
        {
            OnlineUserDelete(user);
        }
        free(header);
    }
    return 0;
}