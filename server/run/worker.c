#include <stdio.h>
#include <stdlib.h>
#include <logger.h>
#include <protocol/base.h>
#include <run/worker.h>
#include <signal.h>

#include "run/user.h"
#include "run/jobs.h"


void *WorkerMain(void *arg)
{
    char workerName[10];
    CRPBaseHeader *header;
    POnlineUser user;
    WorkerType *worker = (WorkerType *) arg;

    sprintf(workerName, "WORKER-%d", worker->workerId);
    signal(SIGINT, SIG_IGN);
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
            log_info("Packet", "Processing %x\n", header->packetID);
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
    }
    log_info(workerName, "Exit.\n");
    return 0;
}