#include <stdio.h>
#include <stdlib.h>
#include <logger.h>

#include "run/worker.h"
#include "run/user.h"
#include "run/jobs.h"


void *WorkerMain(void *arg)
{
    char workerName[10];
    CRPBaseHeader *header;
    POnlineUser user;
    WorkerType *worker = (WorkerType *) arg;

    sprintf(workerName, "WORKER-%d", worker->workerId);
    while (IsServerRunning)
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
    log_info(workerName, "Exit.\n");
    return 0;
}