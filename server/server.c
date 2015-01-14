#include "server.h"
#include "datafile/base.h"
#include <stdlib.h>
#include <logger.h>
#include <signal.h>
#include <stdio.h>
#include <run/gc.h>
#include <unistd.h>
#include "run/jobs.h"

int IsServerRunning = 1;
WorkerType worker[CONFIG_WORKER_COUNT];

pthread_t ThreadListener;

static void initWorker(int id, WorkerType *worker)
{
    worker->workerId = id;
    pthread_create(&worker->WorkerThread, NULL, WorkerMain, worker);
}

static void sigInterupt(int sig)
{
    log_info("MAIN", "Server is exiting...\n");
    IsServerRunning = 0;
    struct sigaction act = {
            .sa_handler=SIG_DFL,
    };
    sigaction(sig, &act, NULL);
}

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    struct sigaction act = {
            .sa_flags=0
    };
    sigemptyset(&act.sa_mask);
    act.sa_handler = sigInterupt;
    sigaction(SIGINT, &act, NULL);
    act.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &act, NULL);//忽略Socket PIPE Error 信号
    if (!DataModuleInit())
    {
        log_error("MAIN", "Fail to initliaze data module.\n");
        return EXIT_FAILURE;
    }

    JobManagerInitialize();

    for (int i = 0; i < CONFIG_WORKER_COUNT; i++)
    {
        initWorker(i, worker + i);
    }
    InitUserManager();

    pthread_create(&ThreadListener, NULL, ListenMain, NULL);
    GarbageCollectorInitialize();
    while (IsServerRunning)
    {
        pause();
    }
    log_info("MAIN", "Stopping Listener\n");
    pthread_cancel(ThreadListener);
    pthread_join(ThreadListener, NULL);

    for (int i = 0; i < CONFIG_WORKER_COUNT; ++i)
    {
        log_info("MAIN", "Stopping Worker %d\n", worker[i].workerId);
        pthread_cancel(worker[i].WorkerThread);
        pthread_join(worker[i].WorkerThread, NULL);
    }
    log_info("MAIN", "Cleanup Modules\n");

    GarbageCollectorFinalize();
    UserManagerFinalize();
    JobManagerFinalize();
    DataModuleFinalize();
    return EXIT_SUCCESS;
}