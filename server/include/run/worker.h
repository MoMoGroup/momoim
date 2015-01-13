#pragma once

#include <semaphore.h>
#include <pthread.h>

//事务处理器
typedef struct
{
    int workerId;

    pthread_t WorkerThread;

} WorkerType;

//事务处理器主线程
void *WorkerMain(void *arg);