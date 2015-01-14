#include <stddef.h>
#include <imcommon/recursive.h>
#include <stdlib.h>
#include <assert.h>
#include <asm-generic/errno-base.h>

RecursiveMutex RecursiveMutexInit()
{
    RecursiveMutex lock = (RecursiveMutex) malloc(sizeof(__RecursiveMutexLock));
    if (lock == NULL)
    {
        return NULL;
    }
    lock->nRef = 0;
    lock->owner = 0;
    if (pthread_mutex_init(&lock->mutex, NULL) == 0)
    {
        return lock;
    }
    else
    {
        free(lock);
        return NULL;
    }
}

int RecursiveMutexDestory(RecursiveMutex lock)
{
    int ret = pthread_mutex_destroy(&lock->mutex);
    if (ret == 0)
    {
        free(lock);
    }
    return ret;
}

void RecursiveMutexLock(RecursiveMutex p)
{
    pthread_t self = pthread_self();
    if (p->nRef > 0 && pthread_equal(p->owner, self))
    {
        p->nRef++;
    }
    else
    {
        pthread_mutex_lock(&p->mutex);
        assert(p->nRef == 0);
        p->owner = self;
        p->nRef = 1;
    }
}

int RecursiveMutexTry(RecursiveMutex p)
{
    int rc;
    pthread_t self = pthread_self();
    if (p->nRef > 0 && pthread_equal(p->owner, self))
    {
        p->nRef++;
        rc = 0;
    }
    else if (pthread_mutex_trylock(&p->mutex) == 0)
    {
        assert(p->nRef == 0);
        p->owner = self;
        p->nRef = 1;
        rc = 0;
    }
    else
    {
        rc = EBUSY;
    }
    return rc;
}

int RecursiveMutexUnlock(RecursiveMutex p)
{
    p->nRef--;
    if (p->nRef == 0)
    {
        p->owner = 0;
        pthread_mutex_unlock(&p->mutex);
    }

    return 0;
}
