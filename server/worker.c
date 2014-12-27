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

        header = CRPRecv(user->sockfd);
        if (header == NULL || processUser(user, header) == 0)
        {
            OnlineUserDelete(user);
        }
        else
        {
            UserJoinToPoll(user);
        }
        free(header);
    }
    return 0;
}