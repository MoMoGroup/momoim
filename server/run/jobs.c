#include <semaphore.h>
#include <server.h>
#include "run/jobs.h"

static POnlineUser jobQueue[CONFIG_JOB_QUEUE_SIZE];
static POnlineUser *pJobQueueHead = jobQueue, *pJobQueueTail = jobQueue + 1;
static pthread_mutex_t jobLock;
static pthread_cond_t cond;

void JobManagerKick(POnlineUser user)
{
    pthread_mutex_lock(&jobLock);
    POnlineUser *p = pJobQueueHead;
    while (!(p + 1 == pJobQueueTail ||
             p + 1 == pJobQueueTail + (sizeof(jobQueue) / sizeof(*jobQueue))))
    {
        p = jobQueue + (p - jobQueue + 1) % CONFIG_JOB_QUEUE_SIZE;
        if (*p == user)
            *p = NULL;
    }
    pthread_mutex_unlock(&jobLock);
}

POnlineUser JobManagerPop(void)
{
    POnlineUser user;
    pthread_mutex_lock(&jobLock);
    redo:
    while ((pJobQueueHead + 1 == pJobQueueTail) ||
           (pJobQueueHead + 1 == pJobQueueTail + (sizeof(jobQueue) / sizeof(*jobQueue))))
    {
        pthread_cond_wait(&cond, &jobLock);
    }

    int queueFull = pJobQueueHead == pJobQueueTail;

    pJobQueueHead = jobQueue + (pJobQueueHead - jobQueue + 1) % CONFIG_JOB_QUEUE_SIZE;

    user = *pJobQueueHead;
    *pJobQueueHead = NULL;

    if (queueFull)
    {
        pthread_cond_broadcast(&cond);
    }

    if (user == NULL || !OnlineUserHold(user))
        goto redo;
    pthread_mutex_unlock(&jobLock);

    return user;
}

void JobManagerPush(POnlineUser v)
{
    pthread_mutex_lock(&jobLock);

    while (pJobQueueHead == pJobQueueTail)
    {
        pthread_cond_wait(&cond, &jobLock);
    }
    int isEmpty = (pJobQueueHead + 1 == pJobQueueTail) ||
                  (pJobQueueHead + 1 == pJobQueueTail + (sizeof(jobQueue) / sizeof(*jobQueue)));
    *pJobQueueTail = v;

    pJobQueueTail = jobQueue + (pJobQueueTail - jobQueue + 1) % CONFIG_JOB_QUEUE_SIZE;

    if (isEmpty)
    {
        pthread_cond_broadcast(&cond);
    }
    pthread_mutex_unlock(&jobLock);
}

void InitJobManger(void)
{
    pthread_mutex_init(&jobLock, NULL);
    pthread_cond_init(&cond, NULL);
}

