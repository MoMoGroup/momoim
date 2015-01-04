#include <stdio.h>
#include <stdlib.h>
#include <logger.h>
#include <protocol/base.h>

#include "run/worker.h"
#include "run/user.h"
#include "run/jobs.h"


void *WorkerMain(void *arg)
{
    char workerName[10];
    CRPBaseHeader *header;
    POnlineUser user;
    WorkerType *worker = (WorkerType *) arg;
    struct timespec ts, te;

    sprintf(workerName, "WORKER-%d", worker->workerId);
    while (IsServerRunning)
    {
        user = JobManagerPop();

        header = CRPRecv(user->sockfd);//在UserJoinToPoll之前,用户被保持单线程处理状态,这里依然安全
        if (header == NULL)
        {
            OnlineUserDelete(user);
            continue;
        }
        else
        {
            EpollAdd(user);
            clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
            if (ProcessUser(user, header) == 0)
            {
                OnlineUserDelete(user);
                free(header);
                continue;
            }
            clock_gettime(CLOCK_MONOTONIC_COARSE, &te);
            if (te.tv_sec - ts.tv_sec > 1 || te.tv_nsec - ts.tv_nsec > 30000000)
            {
                log_warning("Worker", "Packet %hu too slow.\n", header->packetID);
            }
            free(header);
        }
        UserDrop(user);
    }
    log_info(workerName, "Exit.\n");
    return 0;
}