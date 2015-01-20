#include<stdio.h>
#include <protocol/base.h>
#include <pthread.h>
#include <stdlib.h>
#include <logger.h>
#include "ClientSockfd.h"
#include "MainInterface.h"


static pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;


typedef struct messageloop
{
    uint32_t sessionid;
    uint16_t packetID;           //包ID，用于区分不同数据包

    int  (*fn)(CRPBaseHeader *, void *data);

    void *data;


    struct messageloop *next;
} messageloop;

messageloop messagehead;

//添加一个节点到主消息循环
void AddMessageNode(uint32_t sessionid, int (*fn)(CRPBaseHeader *, void *), void *data)
{
    messageloop *entry = (messageloop *) malloc(sizeof(messageloop));
    entry->sessionid = sessionid;
    entry->fn = fn;
    entry->data = data;
    entry->next = NULL;
    messageloop *p;

    pthread_rwlock_wrlock(&lock);//写锁定
    p = &messagehead;
    while (p->next)
    {
        p = p->next;
    }
    p->next = entry;
    pthread_rwlock_unlock(&lock);//取消锁
}

//删除一个节点
void DeleteMessageNode(uint32_t sessid)
{
    messageloop *p, *delete;
    pthread_rwlock_wrlock(&lock);//写锁定
    p = &messagehead;
    while (p)
    {
        delete = p->next;
        //找到要删除的delete之后,删除
        if ((delete->sessionid == sessid))
        {
            p->next = delete->next;
            free(delete);
        }
        p = p->next;
    }
    pthread_rwlock_unlock(&lock);//取消锁
}

int MessageLoopFunc()
{
    CRPBaseHeader *header;
    while (1)
    {
        header = CRPRecv(sockfd);
        if (header == NULL)
        {
            pthread_cancel(ThreadKeepAlive);
            pthread_join(ThreadKeepAlive, NULL);
            g_idle_add(DestoryAll, "服务器异常关闭");
            CRPClose(sockfd);
            pthread_detach(pthread_self());//安全退出当前线程
            pthread_exit(NULL);
        }
        else
        {
            pthread_rwlock_rdlock(&lock);//读锁定
            messageloop *prev = &messagehead, *p;
            log_info("CRPPacket", "packet id %02hx,session id %u\n", header->packetID, header->sessionID);
            int flag = 1;
            while (prev->next)
            {
                p = prev->next;
                if (p->sessionid == header->sessionID)
                {
                    break;

                }
                prev = prev->next;
            }
            pthread_rwlock_unlock(&lock);//取消锁

            if (prev->next)
            {
                flag = p->fn(header, p->data);//执行此节点的函数，并拿到返回值
            }

            if (flag == 0)//根据返回值判断是否需要删除此节点
            {
                pthread_rwlock_wrlock(&lock);//写锁定
                prev->next = p->next;
                free(p);
                pthread_rwlock_unlock(&lock);//取消锁
            }
            free(header);
        }
    }
    return 0;
}