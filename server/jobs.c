#include <semaphore.h>
#include <server.h>
#include <jobs.h>

OnlineUser *jobQueue[MAX_CLIENTS];
OnlineUser **pJobQueueHead = jobQueue, **pJobQueueTail = jobQueue;
sem_t SemJobGold, SemJobAir;
pthread_mutex_t lock;

OnlineUser *PollJob(void)
{
    OnlineUser *user;
    sem_wait(&SemJobGold);  //等待房子里有金子
    pthread_mutex_lock(&lock);//冲入房子
    user = *pJobQueueHead;  //从房子里抢走一块金子，

    if (pJobQueueHead == jobQueue + MAX_CLIENTS)//计算下一个应该出现金子的位置
    {
        pJobQueueHead = jobQueue;
    }
    else
    {
        pJobQueueHead++;
    }
    pthread_mutex_unlock(&lock);//走出房子
    sem_post(&SemJobAir);   //在房子里留下块空气
    return user;
}

void PushJob(OnlineUser *v)
{
    sem_wait(&SemJobAir);//等待房子里出现空气
    pthread_mutex_lock(&lock);//进入房子
    *pJobQueueTail = v;//将金子扔到房子里

    if (pJobQueueTail == jobQueue + MAX_CLIENTS)//计算下一个应该出现空气的位置
    {
        pJobQueueTail = jobQueue;
    }
    else
    {
        pJobQueueTail++;
    }

    pthread_mutex_unlock(&lock);//走出房子
    sem_post(&SemJobGold);//房子里多了块金子
}

void InitJobManger(void)
{
    sem_init(&SemJobGold, 0, 0);    //初始情况房子里没有金子
    sem_init(&SemJobAir, 0, sizeof(jobQueue) / sizeof(*jobQueue));//只有N个空气
    pthread_mutex_init(&lock, NULL);
}

