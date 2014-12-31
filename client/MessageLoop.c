#include<stdio.h>
#include <protocol/base.h>
#include <pthread.h>
#include <stdlib.h>
#include "ClientSockfd.h"


pthread_rwlock_t lock;


typedef struct messageloop {
    uint32_t sessionid;
    uint16_t packetID;           //包ID，用于区分不同数据包

    int  (*fn)(CRPBaseHeader *, void *data);

    void *data;


    struct messageloop *next;
} messageloop;

messageloop *messagehead;

void AddMessageNode(uint32_t sessionid, uint16_t packetID, int  (*fn)(CRPBaseHeader *, void *data), void *data)
{
    messageloop *entry = (messageloop *) malloc(sizeof(messageloop));
    entry->packetID = packetID;
    entry->sessionid = sessionid;
    entry->fn = fn;
    entry->data = data;
    entry->next = NULL;
    messageloop *p;

    pthread_rwlock_wrlock(&lock);//写锁定
    p = messagehead;
    while (p->next)
    {
        p = p->next;
    }
    p->next = entry;
    pthread_rwlock_unlock(&lock);//取消锁
}

void DeleteMessageNode(uint32_t sessid, uint16_t packetid)
{
    messageloop *p, *delete;
    pthread_rwlock_wrlock(&lock);//写锁定
    p = messagehead;
    while (p)
    {
        delete = p->next;
        if ((delete->sessionid == sessid && delete->packetID == packetid))//找到要删除的delete之后,删除
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
    pthread_rwlock_init(&lock, NULL);

    messageloop *messagehead;
    messagehead = (messageloop *) calloc(1, sizeof(struct messageloop));//创建头节点

    CRPBaseHeader *header;
    while (1)
    {
        header = CRPRecv(sockfd);
        pthread_rwlock_rdlock(&lock);//写锁定
        messageloop *prev = messagehead, *p;
        int flag = 0;
        while (prev->next)
        {
            p = prev->next;
            if (p->packetID == header->packetID && p->sessionid == header->sessionID)
            {

                if (p->fn(header, p->data))
                {
                    flag = 1;

                }
                break;

            }
            prev = prev->next;

        }
        pthread_rwlock_unlock(&lock);//取消锁

        if (flag == 0)
        {
            pthread_rwlock_wrlock(&lock);//写锁定
            prev->next = p->next;
            free(p);
            pthread_rwlock_unlock(&lock);//取消锁
        }
        free(header);
    }
}