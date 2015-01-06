#include <datafile/message.h>
#include "datafile/base.h"
#include "datafile/user.h"
#include "datafile/friend.h"

int DataModuleInit()
{
    if (!AuthInit())
        return 0;
    if (!UserInit())
    {
        AuthFinalize();
        return 0;
    }
    if (!UserFriendsInit())
    {
        UserFinalize();
        AuthFinalize();
        return 0;
    }
    if (!UserMessagesInit())
    {
        UserFriendsFinalize();
        UserFinalize();
        AuthFinalize();
        return 0;
    }
    return 1;
}

void DataModuleFinalize()
{
    UserMessagesFinalize();
    UserFriendsFinalize();
    UserFinalize();
    AuthFinalize();
}