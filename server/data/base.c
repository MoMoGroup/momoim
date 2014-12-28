#include <data/base.h>
#include <data/user.h>

int DataModuleInit()
{
    if (!AuthInit())
        return 0;
    if (!UserInit())
    {
        AuthFinalize();
        return 0;
    }
    return 1;
}

void DataModuleFinalize()
{
    AuthFinalize();
}