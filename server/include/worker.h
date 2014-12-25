#pragma once

#include <semaphore.h>
#include <pthread.h>

//事务处理器
typedef struct
{
    int workerId;

    pthread_t WorkerThread;

    sem_t ready;
    pthread_mutex_t lock;

} WorkerType;

//事务处理器主线程
void *WorkerMain(void *arg);