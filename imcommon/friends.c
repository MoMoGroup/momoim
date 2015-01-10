#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <imcommon/friends.h>

size_t UserFriendsSize(UserFriends *friends)
{
    size_t length = sizeof(friends->groupCount) +
            friends->groupCount * (uint16_t) (
                    sizeof(*friends->groups) -
                            sizeof(friends->groups->friends)
            );
    for (int i = 0; i < friends->groupCount; ++i)
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
        memcpy(p, group, sizeof(UserGroup) - sizeof(group->friends));//2.Write Group Info (WITHOUT FRIENDS)
        p += sizeof(UserGroup) - sizeof(group->friends);
        for (int i = 0; i < group->friendCount; ++i)
        {
            memcpy(p, group->friends + i, sizeof(*group->friends));  //3.Write Friends
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
    {
        return NULL;
    }
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
                {
                    free(friends->groups[i].friends);
                }//III.Free Friends
            }
            free(friends->groups);//II.Free Group Info
        }

        free(friends);   //I.Free UserFriends Object
    }
}

int UserFriendsGroupDelete(UserFriends *friends, uint8_t groupID)
{
    UserGroup *group = UserFriendsGroupGet(friends, groupID);
    if (group)
    {
        for (int j = (int) (group - friends->groups) + 1; j < friends->groupCount; ++j)
        {
            friends->groups[j - 1] = friends->groups[j];
        }
        --friends->groupCount;
        void *ptr = realloc(friends->groups, sizeof(UserGroup) * friends->groupCount);
        if (ptr != NULL)//如果内存重分配成功,则重设用户组指针.
        {
            friends->groups = ptr;
        }//一般内存减小不会导致内存分配失败,如果内存重分配失败问题也不大.到时候释放内存时还是能够保证内存干净.
        return 1;
    }
    return 0;
}

UserGroup *UserFriendsGroupGet(UserFriends *friends, uint8_t groupID)
{

    for (int i = 0; i < friends->groupCount; ++i)
    {
        UserGroup *group = friends->groups + i;
        if (group->groupId == groupID)
        {
            return group;
        }
    }
    return NULL;
}

UserGroup *UserFriendsGroupAdd(UserFriends *friends, uint8_t groupId, const char *name)
{
    void *ptr;
    if (UserFriendsGroupGet(friends, groupId))
    {
        errno = EEXIST;
        return 0;
    }

    ptr = realloc(friends->groups, (friends->groupCount + 1) * sizeof(UserGroup));
    if (ptr == NULL)//如果内存重分配失败,操作无法继续.
    {
        errno = ENOMEM;
        return 0;
    }
    friends->groups = ptr;
    friends->groups[friends->groupCount] = friends->groups[friends->groupCount - 1];//Pending组保持最后一组状态
    UserGroup *group = friends->groups + friends->groupCount - 1;
    group->friendCount = 0;
    group->friends = NULL;
    group->groupId = groupId;
    size_t nameLen = strlen(name);
    if (nameLen > 63)
    {
        nameLen = 63;
    }
    memcpy(group->groupName, name, nameLen);
    group->groupName[nameLen] = 0;
    ++friends->groupCount;
    return group;
}

int UserFriendsUserAdd(UserGroup *group, uint32_t user)
{
    void *ptr;
    for (int i = 0; i < group->friendCount; ++i)
    {
        if (group->friends[i] == user)//好友ID已存在,失败返回.
        {
            errno = EEXIST;
            return 0;
        }
    }
    ptr = realloc(group->friends, (group->friendCount + 1) * sizeof(uint32_t));
    if (ptr == NULL)//如果内存重分配失败,操作无法继续.
    {
        errno = ENOMEM;
        return 0;
    }
    group->friends = ptr;
    ++group->friendCount;
    group->friends[group->friendCount - 1] = user;
    return 1;
}

int UserFriendsUserDelete(UserGroup *group, uint32_t user)
{
    void *ptr;
    for (int i = 0; i < group->friendCount; ++i)
    {
        if (group->friends[i] == user)
        {
            for (int j = i + 1; j < group->friendCount; ++j)
            {
                group->friends[j - 1] = group->friends[j];
            }
            --group->friendCount;
            if (group->friendCount == 0)
            {
                free(group->friends);
                group->friends = NULL;
            }
            else
            {
                ptr = realloc(group->friends, sizeof(uint32_t) * group->friendCount);
                if (ptr != NULL)//如果内存重分配成功,则重设用户组指针.
                {
                    group->friends = ptr;
                }//一般内存减小不会导致内存分配失败,如果内存重分配失败问题也不大.到时候释放内存时还是能够保证内存干净.
            }
            return 1;
        }
    }
    return 0;
}

int UserFriendsJoin(UserFriends *friends, uint8_t groupId, uint32_t uid)
{
    UserGroup *group = UserFriendsGroupGet(friends, groupId);
    if (!group)
    {
        return 0;
    }
    return UserFriendsUserAdd(group, uid);
}

int UserFriendsUserMove(UserGroup *src, UserGroup *dst, uint32_t uid)
{
    if (!UserFriendsUserDelete(src, uid))
    {
        return 0;
    }
    if (!UserFriendsUserAdd(dst, uid))
    {
        UserFriendsUserAdd(src, uid);//尝试再给它搞回去..
        return 0;
    }
    return 1;
}
