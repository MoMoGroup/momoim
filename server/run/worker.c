#include <stdio.h>
#include <stdlib.h>
#include <logger.h>
#include <protocol/base.h>
#include <run/worker.h>

#include "run/user.h"
#include "run/jobs.h"


void *WorkerMain(void *arg)
{
    char workerName[10];
    CRPBaseHeader *header;
    POnlineUser user;
    WorkerType *worker = (WorkerType *) arg;

    sprintf(workerName, "WORKER-%d", worker->workerId);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (IsServerRunning)
    {
        if (worker->workerId == 0)
        {
            if (random() / (double) RAND_MAX < CONFIG_GC_RADIO)
            {
                UserGC();
            }
        }
        user = JobManagerPop();
        if (!user && !IsServerRunning)
        {
            break;
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        header = CRPRecv(user->crp);
        if (header == NULL)
        {
            OnlineUserDelete(user);
            continue;
        }
        else
        {
            if (user->state == OUS_ONLINE)
            {
                time(&user->lastUpdateTime);
            }
            EpollAdd(user);
            if (ProcessUser(user, header) == 0)
            {
                OnlineUserDelete(user);
                free(header);
                continue;
            }
            free(header);
        }
        UserDrop(user);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }
    log_info(workerName, "Exit.\n");
    return 0;
}