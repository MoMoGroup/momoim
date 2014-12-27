#include <string.h>
#include <stdlib.h>
#include "imcommon/friends.h"

size_t UserFriendsSize(UserFriends *friends)
{
    size_t length = sizeof(friends->groupCount) +
                    friends->groupCount * (uint16_t) (
                            sizeof(friends->groups) -
                            sizeof(friends->groups->friends)
                    );
    for (int i = 0; i < length; ++i)
    {
        length += friends->groups[i].friendCount * sizeof(*(friends->groups[i].friends));
    }
    return length;
}

int UserFriendsEncode(UserFriends *friends, unsigned char *p)
{
    memcpy(p, &friends->groupCount, sizeof(friends->groupCount));   //1.Write Group Count
    p += sizeof(friends->groupCount);
    for (int j = 0; j < friends->groupCount; ++j)
    {
        UserGroup *group = friends->groups + j;
        memcpy(p, &group, sizeof(UserGroup) - sizeof(group->friends));//2.Write Group Info (WITHOUT FRIENDS)
        for (int i = 0; i < group->friendCount; ++i)
        {
            memcpy(p, &group->friends[i], sizeof(*group->friends));  //3.Write Friends
            p += sizeof(*group->friends);
        }
    }
    return 0;
}

UserFriends *UserFriendsDecode(unsigned char *p)
{
    UserFriends *friends = NULL;

    //I.Alloc Object
    friends = (UserFriends *) malloc(sizeof(UserFriends));
    if (friends == NULL)
        return NULL;
    memcpy(&friends->groupCount, p, sizeof(friends->groupCount));   //1.Read group count
    p += sizeof(friends->groupCount);

    //II.Alloc Group
    friends->groups = (UserGroup *) calloc(friends->groupCount, sizeof(UserGroup));
    if (friends->groups == NULL)
    {
        UserFriendsFree(friends);
        friends = NULL;
        goto fail;
    }
    for (int i = 0; i < friends->groupCount; ++i)
    {
        UserGroup *group = friends->groups + i;
        memcpy(group, p, sizeof(UserGroup) - sizeof(((UserGroup *) 0)->friends));   //2. Read Group Info (WITHOUT FRIENDS)
        p += sizeof(UserGroup) - sizeof(((UserGroup *) 0)->friends);

        //III.Alloc Friends
        group->friends = (uint32_t *) malloc(sizeof(*group->friends) * friends->groups[i].friendCount);
        if (group->friends == NULL)
        {
            goto fail;
        }

        memcpy(group->friends, p, sizeof(uint32_t) * group->friendCount);   //3.Read Friends
        p += sizeof(uint32_t) * group->friendCount;
    }
    return friends;
    fail:
    UserFriendsFree(friends);
    return NULL;
}

void UserFriendsFree(UserFriends *friends)
{

    if (friends)
    {
        if (friends->groups)
        {
            for (int i = 0; i < friends->groupCount; ++i)
            {
                if (friends->groups[i].friends)
                    free(friends->groups[i].friends);//III.Free Friends
            }
            free(friends->groups);//II.Free Group Info
        }

        free(friends);   //I.Free UserFriends Object
    }
}