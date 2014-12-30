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
        user = PullJob();
        if (OnlineUserHold(user))   //保持用户,使用户对象在整个代码执行过程中不会被释放
        {
            header = CRPRecv(user->sockfd);//在UserJoinToPoll之前,用户被保持单线程处理状态
            UserJoinToPool(user);

            if (header == NULL || ProcessUser(user, header) == 0)
            {
                OnlineUserDelete(user);
            }
            OnlineUserUnhold(user);
            free(header);
        }
    }
    return 0;
}