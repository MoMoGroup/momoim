#pragma once

#include "stddef.h"
#include "stdint.h"

typedef struct __attribute__ ((packed))
{
    uint32_t uid;
    char sex;
    char nickName[32];
    unsigned char icon[16];
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

extern size_t UserFriendsSize(UserFriends *friends);

extern int UserFriendsEncode(UserFriends *from, unsigned char *to);

extern UserFriends *UserFriendsDecode(unsigned char *from);

extern void UserFriendsFree(UserFriends *friends);