#pragma once

typedef struct structHostDiscoverEntry
{
    char key[32];

    int(*fn)(const struct sockaddr_in *addr, void *data);

    void *data;

    struct structHostDiscoverEntry *prev, *next;
} HostDiscoverEntry;
typedef struct structHostDiscoverTable
{
    HostDiscoverEntry *first, *last;
} HostDiscoverTable;

int NatHostInitlize();
void NatHostFinalize();
int NatHostDiscoverNotify(struct sockaddr_in const *address, const uint8_t key[32]);

int NatHostDiscoverUnregister(HostDiscoverEntry *entry);

HostDiscoverEntry *NatHostDiscoverRegister(const uint8_t key[32],
                                           int(*fn)(struct sockaddr_in const *, void *),
                                           void *data);