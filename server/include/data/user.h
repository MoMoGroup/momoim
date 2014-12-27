#pragma once

typedef struct __attribute__ ((packed))
{
    uint32_t uid;
    char nickName[32];
    char sex;
} UserInfo;

typedef struct __attribute__ ((packed))
{
    char groupName[64];
    uint16_t friendCount;
    uint32_t *friends;
} UserGroup;
typedef struct __attribute__ ((packed))
{
    uint16_t groupCount;
    UserGroup *groups;
} UserFriends;

void UserGetDir(char *path, uint32_t uid, const char *relPath);

void UserCreateDirectory(uint32_t uid);

//Info
void UserCreateInfoFile(uint32_t uid, char *path);

UserInfo *UserGetInfo(uint32_t uid);

//Groups
void UserCreateFriendsFile(char *path);

void UserGetGroups(uint32_t uid);