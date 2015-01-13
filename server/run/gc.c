#include <pthread.h>
#include <server.h>
#include <signal.h>
#include <unistd.h>
#include "run/gc.h"

pthread_t thread_gc;

static void *garbageCollector(void *pointer)
{
    signal(SIGINT, SIG_IGN);
    while (IsServerRunning)
    {
        sleep(5 * 60);
        //GC
        UserGC();
    }
}

void GarbageCollectorInitialize()
{
    pthread_create(&thread_gc,
                   NULL,
                   garbageCollector,
                   NULL);
}

void GarbageCollectorFinalize()
{
    pthread_kill(&thread_gc, SIGINT);
}
