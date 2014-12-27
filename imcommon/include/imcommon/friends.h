#pragma once

#include "stddef.h"
#include "stdint.h"

typedef struct __attribute__ ((packed))
{
    uint32_t uid;
    char nickName[32];
    char sex;
} UserInfo;

typedef struct __attribute__ ((packed))
{
    uint8_t groupId;
    char groupName[64];
    uint16_t friendCount;

    uint32_t *friends;//(Last)
} UserGroup;
typedef struct __attribute__ ((packed))
{
    uint16_t groupCount;
    UserGroup *groups;
} UserFriends;

size_t UserFriendsSize(UserFriends *friends);

int UserFriendsEncode(UserFriends *from, unsigned char *to);

UserFriends *UserFriendsDecode(unsigned char *from);

void UserFriendsFree(UserFriends *friends);