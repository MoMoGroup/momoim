#include <data/base.h>

int DataModuleInit()
{
    if (!AuthInit())
        return 0;
    return 1;
}

void DataModuleFinalize()
{
    AuthFinalize();
}