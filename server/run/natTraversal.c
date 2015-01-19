#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <logger.h>
#include <arpa/inet.h>
#include "run/natTraversal.h"

static HostDiscoverTable table;
static pthread_rwlock_t lock;

int NatHostInitlize()
{
    pthread_rwlock_init(&lock, NULL);
    return 1;
}

void NatHostFinalize()
{
    pthread_rwlock_destroy(&lock);
}

HostDiscoverEntry *NatHostDiscoverRegister(const uint8_t key[32],
                                           int(*fn)(struct sockaddr_in const *, void *),
                                           void *data)
{
    char hexKey[65] = {0};
    for (int i = 0; i < 32; ++i)
    {
        sprintf(hexKey + i * 2, "%02x", (int) key[i]);
    }
    HostDiscoverEntry *entry = (HostDiscoverEntry *) malloc(sizeof(HostDiscoverEntry));
    entry->fn = fn;
    entry->data = data;
    memcpy(entry->key, key, 32);
    pthread_rwlock_wrlock(&lock);
    entry->prev = table.last;
    entry->next = NULL;
    if (table.last == NULL)
    {
        table.first = table.last = entry;
    }
    else
    {
        table.last->next = entry;
        table.last = entry;
    }
    pthread_rwlock_unlock(&lock);
    return entry;
}

int NatHostDiscoverUnregister(HostDiscoverEntry *entry)
{
    char hexKey[65] = {0};
    for (int i = 0; i < 32; ++i)
    {
        sprintf(hexKey + i * 2, "%02x", (int) entry->key[i]);
    }
    pthread_rwlock_wrlock(&lock);
    if (entry->prev)
    {
        entry->prev->next = entry->next;
    }
    else if (entry == table.first)
    {
        table.first = entry->next;
    }
    if (entry->next)
    {
        entry->next->prev = entry->prev;
    }
    else if (table.last == entry)
    {
        table.last = entry->prev;
    }
    free(entry);
    pthread_rwlock_unlock(&lock);
    return 1;
}

int NatHostDiscoverNotify(struct sockaddr_in const *address, const uint8_t key[32])
{
    char hexKey[65] = {0};
    for (int i = 0; i < 32; ++i)
    {
        sprintf(hexKey + i * 2, "%02x", (int) key[i]);
    }
    log_info("NatDiscover", "Notify:Key:%s\n", hexKey);
    HostDiscoverEntry *entry = NULL;
    pthread_rwlock_rdlock(&lock);
    for (entry = table.first; entry != NULL; entry = entry->next)
    {
        if (memcmp(key, entry->key, 32) == 0)
        {
            break;
        }
    }
    pthread_rwlock_unlock(&lock);
    int ret = 0;
    if (entry)
    {
        log_info("NatNotify", "Key:%s,IP:%s\n", hexKey, inet_ntoa(address->sin_addr));
        ret = entry->fn(address, entry->data);
        NatHostDiscoverUnregister(entry);
    }
    return ret;
}
