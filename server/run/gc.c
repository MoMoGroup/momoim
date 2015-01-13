#include "run/gc.h"


/*
static void *garbageCollector(void *pointer)
{
    signal(SIGINT, SIG_IGN);
    while (IsServerRunning)
    {
        sleep(CONFIG_GC_TIME);
        //GC
        UserGC();
    }
}
*/

void GarbageCollectorInitialize()
{
}

void GarbageCollectorFinalize()
{
}
