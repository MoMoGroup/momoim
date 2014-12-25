#include <semaphore.h>
#include <server.h>
#include <jobs.h>

OnlineUser *jobQueue[MAX_CLIENTS];
OnlineUser **pJobQueueHead = jobQueue, **pJobQueueTail = jobQueue;
sem_t SemaphoreJobQueue;
pthread_mutex_t lock;

OnlineUser *PollJob(void)
{
    OnlineUser *user;
    sem_wait(&SemaphoreJobQueue);
    pthread_mutex_lock(&lock);
    user = *pJobQueueHead;

    if (pJobQueueHead == jobQueue + MAX_CLIENTS)
    {
        pJobQueueHead = jobQueue;
    }
    else
    {
        pJobQueueHead++;
    }

    pthread_mutex_unlock(&lock);
    return user;
}

void PushJob(OnlineUser *v)
{
    pthread_mutex_lock(&lock);
    *pJobQueueTail = v;

    if (pJobQueueTail == jobQueue + MAX_CLIENTS)
    {
        pJobQueueTail = jobQueue;
    }
    else
    {
        pJobQueueTail++;
    }

    pthread_mutex_unlock(&lock);
    sem_post(&SemaphoreJobQueue);

}

void InitJobManger(void)
{
    sem_init(&SemaphoreJobQueue, 0, 0);
    pthread_mutex_init(&lock, NULL);
}

