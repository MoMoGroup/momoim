#include <pthread.h>
#include "p2p.h"

static pthread_t sockThread;

void P2PFinal()
{
    sockThread = 0;

}

void *listenerStart(void *data)
{
    while (sockThread) {

    }
    pthread_exit(NULL);
}

int P2PInit()
{
    pthread_create(&sockThread, NULL, listenerStart, NULL);

    return 0;
}
