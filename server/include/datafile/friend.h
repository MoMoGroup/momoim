#pragma once

#include<pthread.h>
#include "imcommon/friends.h"

typedef struct
{
    UserFriends *friends;
    pthread_rwlock_t lock, refLock;

} UserFriendsEntry;
typedef struct structUserFriendsTable
{
    UserFriendsEntry *entry;
    struct structUserFriendsTable *next[0x10];
} UserFriendsTable;

//Friends-Groups
int UserFriendsInit();

void UserFriendsFinalize();

void UserFriendsCreate(uint32_t uid);

int UserFriendsSave(uint32_t uid, UserFriends *friends);

UserFriendsEntry *UserFriendsEntryGet(uint32_t uid);

UserFriendsEntry *UserFriendsEntrySet(uint32_t uid, UserFriends *friends);

UserFriends *UserFriendsGet(uint32_t uid, pthread_rwlock_t **lock);

void UserFriendsDrop(uint32_t uid);