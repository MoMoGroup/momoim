#pragma once

#include <semaphore.h>
#include <pthread.h>


typedef struct
{
    int workerId;

    pthread_t WorkerThread;

    sem_t ready;
    pthread_mutex_t lock;

} WorkerType;

void *WorkerMain(void *arg);