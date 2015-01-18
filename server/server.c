#include "server.h"
#include <stdlib.h>
#include <logger.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <datafile/init.h>
#include "run/jobs.h"

int IsServerRunning = 1;
WorkerType PacketWorker[CONFIG_WORKER_COUNT];

pthread_t ThreadListener;

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
    InitServerDataDirectory();
    {
        struct sigaction act = {
                .sa_flags=0
        };
        sigemptyset(&act.sa_mask);
        act.sa_handler = sigInterupt;
        sigaction(SIGINT, &act, NULL);
        act.sa_handler = SIG_IGN;
        sigaction(SIGPIPE, &act, NULL);//忽略Socket PIPE Error 信号
    }

    if (!DataModuleInit())
    {
        log_error("MAIN", "Fail to initliaze data module.\n");
        return EXIT_FAILURE;
    }

    JobManagerInitialize();

    InitUserManager();
    for (int i = 0; i < CONFIG_WORKER_COUNT; i++)
    {
        PacketWorker[i].workerId = i;
        pthread_create(&PacketWorker[i].WorkerThread, NULL, WorkerMain, PacketWorker + i);
    }

    pthread_create(&ThreadListener, NULL, ListenMain, NULL);
    while (IsServerRunning)
    {
        pause();
    }
    log_info("MAIN", "Stopping Listener\n");
    pthread_cancel(ThreadListener);
    pthread_join(ThreadListener, NULL);

    for (int i = 0; i < CONFIG_WORKER_COUNT; ++i)
    {
        log_info("MAIN", "Stopping Worker %d\n", PacketWorker[i].workerId);
        pthread_cancel(PacketWorker[i].WorkerThread);
        //pthread_join(PacketWorker[i].WorkerThread, NULL);
    }
    log_info("MAIN", "UserManagerFinalize\n");
    UserManagerFinalize();
    log_info("MAIN", "JobManagerFinalize\n");
    JobManagerFinalize();
    log_info("MAIN", "DataModuleFinalize\n");
    DataModuleFinalize();
    return EXIT_SUCCESS;
}