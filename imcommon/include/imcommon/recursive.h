#pragma once

#include <pthread.h>

typedef struct
{
    pthread_mutex_t mutex;
    pthread_t owner;
    int nRef;
} __RecursiveMutexLock;
typedef __RecursiveMutexLock *RecursiveMutex;

RecursiveMutex RecursiveMutexInit();

int RecursiveMutexDestory(RecursiveMutex lock);

void RecursiveMutexLock(RecursiveMutex lock);

int RecursiveMutexTry(RecursiveMutex lock);

int RecursiveMutexUnlock(RecursiveMutex lock);
