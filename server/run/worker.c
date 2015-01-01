#include <stdio.h>
#include <stdlib.h>

#include "run/worker.h"
#include "run/user.h"
#include "run/jobs.h"


void *WorkerMain(void *arg)
{
    char workerName[10];
    CRPBaseHeader *header;
    OnlineUser *user;
    WorkerType *worker = (WorkerType *) arg;

    sprintf(workerName, "WORKER-%d", worker->workerId);
    while (!server_exit)
    {
        user = JobManagerPop();

        if (user->status == OUS_PENDING_INIT)
        {
            OnlineUserInit(user);
        }

        header = CRPRecv(user->sockfd);//在UserJoinToPoll之前,用户被保持单线程处理状态,这里依然安全
        if (header == NULL)
        {
            OnlineUserDelete(user);
            continue;
        }
        else
        {
            UserJoinToPool(user);
            if (ProcessUser(user, header) == 0)
            {
                OnlineUserDelete(user);
                free(header);
                continue;
            }
            free(header);
        }
        OnlineUserDrop(user);
    }
    return 0;
}