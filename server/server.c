#include "server.h"
#include "data/base.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <logger.h>
#include "run/jobs.h"

volatile int server_exit = 0;
WorkerType worker[WORKER_COUNT];

pthread_t ThreadListener;

void initWorker(int id, WorkerType *worker)
{
    worker->workerId = id;
    sem_init(&worker->ready, 0, 0);
    pthread_mutex_init(&worker->lock, NULL);
    pthread_create(&worker->WorkerThread, NULL, WorkerMain, worker);
}
int main(int argc, char **argv)
{
    if (!DataModuleInit())
    {
        log_error("SERVER-MAIN", "Fail to initliaze data module.\n");
        return EXIT_FAILURE;
    }

    InitJobManger();

    int i;
    for (i = 0; i < WORKER_COUNT; i++)
    {
        initWorker(i, worker + i);
    }
    InitUserManager();
    pthread_create(&ThreadListener, NULL, ListenMain, NULL);
    pthread_join(ThreadListener, NULL);
    FinalizeUserManager();
    DataModuleFinalize();
    return EXIT_SUCCESS;

}