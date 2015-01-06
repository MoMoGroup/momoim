#pragma once

#include <stdint.h>
#include "imcommon/friends.h"
#include "imcommon/message.h"

typedef struct
{
    UserFriends *friends;
    pthread_rwlock_t lock;
    pthread_mutex_t refLock;
    int refCount;

} UserFriendsEntry;
typedef struct structUserFriendsTable
{
    UserFriendsEntry *entry;
    struct structUserFriendsTable *next[0x10];
} UserFriendsTable;

int UserInit();

void UserFinalize();

void UserGetDir(char *path, uint32_t uid, const char *relPath);

void UserCreateDirectory(uint32_t uid);

int UserQueryByNick(const char *text, uint8_t page, uint8_t count, uint32_t *uids);

//Info
void UserInfoCreate(uint32_t uid);

int UserInfoSave(uint32_t uid, UserInfo *info);

UserInfo *UserInfoGet(uint32_t uid);

void UserInfoFree(UserInfo *info);

//Friends-Groups
void UserFriendsCreate(uint32_t uid);

int UserFriendsSave(uint32_t uid, UserFriends *friends);

UserFriendsEntry *UserFriendsEntryGet(uint32_t uid);

UserFriendsEntry *UserFriendsEntrySet(uint32_t uid, UserFriends *friends);

UserFriends *UserFriendsGet(uint32_t uid, pthread_rwlock_t **lock);

void UserFriendsDrop(uint32_t uid);

int UserMessageFileCreate(uint32_t uid);

MessageFile *UserMessageFileOpen(uint32_t uid);