#include <semaphore.h>
#include <server.h>
#include "run/jobs.h"

static POnlineUser jobQueue[CONFIG_JOB_QUEUE_SIZE];
static POnlineUser *pJobQueueHead = jobQueue, *pJobQueueTail = jobQueue + 1;
static pthread_mutex_t jobLock;
static pthread_cond_t cond;
struct timespec lastPushBegin, lastPushEnd;

void JobManagerKick(POnlineUser user)
{
    pthread_mutex_lock(&jobLock);
    POnlineUser *p = pJobQueueHead;
    while (!(p + 1 == pJobQueueTail ||
            p + 1 == pJobQueueTail + (sizeof(jobQueue) / sizeof(*jobQueue)))) {
        p = jobQueue + (p - jobQueue + 1) % CONFIG_JOB_QUEUE_SIZE;
        if (*p == user)
            *p = NULL;
    }
    pthread_mutex_unlock(&jobLock);
}

POnlineUser JobManagerPop(void)
{
    struct timespec lastPop;
    POnlineUser user;
    pthread_mutex_lock(&jobLock);
    redo:
    while ((pJobQueueHead + 1 == pJobQueueTail) ||
            (pJobQueueHead + 1 == pJobQueueTail + (sizeof(jobQueue) / sizeof(*jobQueue)))) {
        pthread_cond_wait(&cond, &jobLock);     //等待队列非空
    }

    int queueFull = pJobQueueHead == pJobQueueTail;

    pJobQueueHead = jobQueue + (pJobQueueHead - jobQueue + 1) % CONFIG_JOB_QUEUE_SIZE;//插入节点
    user = *pJobQueueHead;
    *pJobQueueHead = NULL;

    if (queueFull) {
        pthread_cond_broadcast(&cond);          //如果Pop之前队列是满的,现在已经不满了,通知Push操作.
    }

    if (user == NULL || !UserHold(user))  //如果得到的对象为空或者无法保持用户(用户正在被删除)
        goto redo;                              //重新选择下一个事务

    clock_gettime(CLOCK_MONOTONIC, &lastPop);
    pthread_mutex_unlock(&jobLock);
    return user;
}

void JobManagerPush(POnlineUser v)
{
    pthread_mutex_lock(&jobLock);

    clock_gettime(CLOCK_MONOTONIC, &lastPushBegin);
    while (pJobQueueHead == pJobQueueTail) {
        pthread_cond_wait(&cond, &jobLock);     //等待队列非满
    }
    int isEmpty = (pJobQueueHead + 1 == pJobQueueTail) ||
            (pJobQueueHead + 1 == pJobQueueTail + (sizeof(jobQueue) / sizeof(*jobQueue)));

    *pJobQueueTail = v;
    pJobQueueTail = jobQueue + (pJobQueueTail - jobQueue + 1) % CONFIG_JOB_QUEUE_SIZE;//移除节点

    if (isEmpty) {
        pthread_cond_broadcast(&cond);          //如果Push之前队列是空的,现在已经非空了,通知Pop操作
    }
    clock_gettime(CLOCK_MONOTONIC, &lastPushEnd);
    pthread_mutex_unlock(&jobLock);
}

void InitJobManger(void)
{
    pthread_mutex_init(&jobLock, NULL);
    pthread_cond_init(&cond, NULL);
}

