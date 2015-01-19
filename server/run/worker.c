#include <stdio.h>
#include <stdlib.h>
#include <logger.h>
#include <protocol/base.h>
#include <run/worker.h>

#include "run/user.h"
#include "run/jobs.h"


void *WorkerMain(void *arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);//允许被取消
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);//允许任意时间被取消
    char workerName[10];
    CRPBaseHeader *header;
    POnlineUser user;
    WorkerType *worker = (WorkerType *) arg;
    int isPendingUser = 0;
    sprintf(workerName, "WORKER-%d", worker->workerId);

    while (IsServerRunning)
    {
        if (worker->workerId == 0)
        {
            if (random() / (double) RAND_MAX < CONFIG_GC_RADIO)
            {
                UserGC();
            }
        }
        isPendingUser = 0;
        user = JobManagerPop();
        if (!user)
        {
            log_warning(workerName, "Interuptted!\n");
            continue;
        }
        header = CRPRecv(user->crp);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);//正在处理数据,此时不允许被取消
        if (header == NULL)
        {
            OnlineUserDelete(user);
            continue;
        }
        else
        {
            if (user->state == OUS_ONLINE)//如果用户当前在线.更新最后一次收到数据包时间
            {
                time(&user->lastUpdateTime);
                EpollAdd(user);//只允许在线用户的数据包并行处理,等待认证用户的数据包只允许串行处理
            }
            else//由于等待用户内存占用量比较少,而且有可能产生realloc导致地址发生变化,所以要标记以特殊处理
            {
                isPendingUser = 1;
            }
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            if (ProcessUser(user, header) == 0)
            {
                OnlineUserDelete(user);
                free(header);
                continue;
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            if (end.tv_sec - start.tv_sec > 0 || end.tv_nsec - start.tv_nsec > 10000000)
            {
                log_warning("PerfMonitor",
                            "Slow Packet %02x,using %ds%dns\n",
                            header->packetID,
                            end.tv_sec - start.tv_sec,
                            end.tv_nsec - start.tv_nsec);
            }
            free(header);
        }
        if (!isPendingUser)//等待用户不允许被Drop
        {
            UserDrop(user);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }
    log_info(workerName, "Exit.\n");
    return 0;
}