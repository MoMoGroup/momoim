#pragma once

typedef struct structHostDiscoverEntry
{
    char key[32];

    int(*fn)(struct sockaddr_in *addr, void *data);

    void *data;

    struct structHostDiscoverEntry *prev, *next;
} HostDiscoverEntry;
typedef struct structHostDiscoverTable
{
    HostDiscoverEntry *first, *last;
} HostDiscoverTable;

int NatHostDiscoverNotify(struct sockaddr_in *address, const char key[32]);

int NatHostDiscoverUnregister(HostDiscoverEntry *entry);

HostDiscoverEntry *NatHostDiscoverRegister(const char key[32], int(*fn)(struct sockaddr_in *, void *), void *data);