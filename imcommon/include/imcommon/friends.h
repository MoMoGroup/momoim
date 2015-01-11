#pragma once

#include <stddef.h>
#include <stdint.h>
#include <time.h>

typedef enum
{
    UGI_BLACKLIST = 0,      //黑名单分组
    UGI_DEFAULT = 1,        //默认好友分组
    UGI_PENDING = UINT8_MAX //等待添加好友分组
} USER_GROUP_ID;
typedef struct __attribute__ ((packed))
{
    uint32_t uid;
    char sex;
    char nickName[32];
    unsigned char icon[16];
    char name[13];
    char birthday[11];
    char constellation;
    char provinces[10];
    char city[35];
    char postcode[7];
    char tel[14];
    char school[28];
    char hometown[51];
    time_t lastlogout;
    uint32_t level;
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

extern int UserFriendsJoin(UserFriends *friends, uint8_t groupId, uint32_t uid);

extern UserGroup *UserFriendsGroupAdd(UserFriends *friends, uint8_t groupId, const char *name);

extern UserGroup *UserFriendsGroupGet(UserFriends *friends, uint8_t groupID);

extern int UserFriendsGroupDelete(UserFriends *friends, uint8_t groupID);

extern int UserFriendsUserAdd(UserGroup *group, uint32_t user);

extern int UserFriendsUserMove(UserGroup *src, UserGroup *dst, uint32_t uid);

extern int UserFriendsUserDelete(UserGroup *group, uint32_t user);

extern void UserFriendsFree(UserFriends *friends);