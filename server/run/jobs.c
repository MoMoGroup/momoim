#include <semaphore.h>
#include <server.h>
#include "run/jobs.h"

static POnlineUser jobQueue[CONFIG_JOB_QUEUE_SIZE];
static POnlineUser *pJobQueueHead = jobQueue, *pJobQueueTail = jobQueue + 1;
static pthread_mutex_t jobLock;
static pthread_cond_t cond;
struct timespec lastPushBegin, lastPushEnd;

//从事务管理器中清除有关该用户的信息
//参数: POnlineUser user 用户
void JobManagerKick(POnlineUser user)
{
    pthread_mutex_lock(&jobLock);
    POnlineUser *p = pJobQueueHead;
    while (!(p + 1 == pJobQueueTail ||
             p + 1 == pJobQueueTail + (sizeof(jobQueue) / sizeof(*jobQueue))))
    {
        p = jobQueue + (p - jobQueue + 1) % CONFIG_JOB_QUEUE_SIZE;
        if (*p == user)
        {
            *p = NULL;
        }
    }
    pthread_mutex_unlock(&jobLock);
}

//从事务管理器中取出一个用户
//返回:成功返回取出的用户,失败返回NULL
//注意,该函数会阻塞线程直到拿到一个用户
//注意,该函数返回NULL时表示服务器正在终止.此时应该主动退出线程
POnlineUser JobManagerPop(void)
{
    POnlineUser user;
    pthread_mutex_lock(&jobLock);
    redo:

    //循环,直到队列非空
    while ((pJobQueueHead + 1 == pJobQueueTail) ||
           (pJobQueueHead + 1 == pJobQueueTail + (sizeof(jobQueue) / sizeof(*jobQueue))))
    {
        pthread_cond_wait(&cond, &jobLock);     //等待队列非空
    }
    //保存队列满状态
    int queueFull = pJobQueueHead == pJobQueueTail;
    //队头向前
    pJobQueueHead = jobQueue + (pJobQueueHead - jobQueue + 1) % CONFIG_JOB_QUEUE_SIZE;
    //取出用户
    user = *pJobQueueHead;
    *pJobQueueHead = NULL;

    if (queueFull)
    {
        pthread_cond_signal(&cond);          //如果Pop之前队列是满的,现在已经不满了,通知Push操作.
    }
    //如果得到的对象为空或者无法保持用户(用户正在被删除)
    if (user == NULL || !OnlineUserHold(user))
    {
        if (!IsServerRunning)//如果服务器正在终止,则直接返回NULL
        {
            pthread_mutex_unlock(&jobLock);
            return NULL;
        }
        //重新选择下一个事务
        goto redo;
    }

    pthread_mutex_unlock(&jobLock);
    return user;
}

//将一个用户加入事务队列
//POnlineUser 要加入的用户
//注意,该函数会阻塞线程直到能够将该用户加入到事务队列中
void JobManagerPush(POnlineUser v)
{
    pthread_mutex_lock(&jobLock);

    clock_gettime(CLOCK_MONOTONIC, &lastPushBegin);
    while (pJobQueueHead == pJobQueueTail)
    {
        pthread_cond_wait(&cond, &jobLock);     //等待队列非满
    }
    int isEmpty = (pJobQueueHead + 1 == pJobQueueTail) ||
                  (pJobQueueHead + 1 == pJobQueueTail + (sizeof(jobQueue) / sizeof(*jobQueue)));

    *pJobQueueTail = v;
    pJobQueueTail = jobQueue + (pJobQueueTail - jobQueue + 1) % CONFIG_JOB_QUEUE_SIZE;//移除节点

    if (isEmpty)
    {
        pthread_cond_signal(&cond);          //如果Push之前队列是空的,现在已经非空了,通知Pop操作
    }
    clock_gettime(CLOCK_MONOTONIC, &lastPushEnd);
    pthread_mutex_unlock(&jobLock);
}
//初始化事务管理器
void JobManagerInitialize(void)
{
    pthread_mutex_init(&jobLock, NULL);
    pthread_cond_init(&cond, NULL);
}
//结束事务管理器
void JobManagerFinalize(void)
{
    pthread_mutex_unlock(&jobLock);
    pthread_mutex_destroy(&jobLock);
    pthread_cond_destroy(&cond);
}