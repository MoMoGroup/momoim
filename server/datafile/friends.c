#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <logger.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <datafile/friend.h>
#include "datafile/user.h"

static UserFriendsTable friendsTable;
static pthread_rwlock_t friendsTableLock;

int UserFriendsInit()
{
    pthread_rwlock_init(&friendsTableLock, NULL);
    return 1;
}

void UserFriendsFinalize()
{
    pthread_rwlock_destroy(&friendsTableLock);
}

void UserFriendsCreate(uint32_t uid)
{
    uint32_t mfriends[] = {
            uid
    };
    UserGroup groups[] = {
            {
                    .groupId=1,
                    .groupName="我的好友",
                    .friendCount=1,
                    .friends=mfriends
            },
            {
                    .groupId=0,
                    .groupName="黑名单",
                    .friendCount=0
            },
            {
                    .groupId=UINT8_MAX,
                    .groupName="Pending",
                    .friendCount=0
            }
    };
    UserFriends friends = {
            .groupCount=3,
            .groups=groups
    };
    UserFriendsSave(uid, &friends);
};

int UserFriendsSave(uint32_t uid, UserFriends *friends)
{

    char path[30];
    UserGetDir(path, uid, "friends");

    int fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0600);
    if (fd == -1)
    {
        log_error("User", "Cannot create user friends file %s.\n", path);
        return 1;
    }
    size_t length = UserFriendsSize(friends);
    if (lseek(fd, length - 1, SEEK_SET) == -1)
    {
        log_error("User", "Cannot expand user friends file %s.\n", path);
        return 3;
    }
    write(fd, "\0", 1);
    void *addr = mmap(NULL, length, PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
        log_error("User", "Cannot mmap user friends file %s.%s\n", path, strerror(errno));
        close(fd);
        return 2;
    }

    UserFriendsEncode(friends, addr);
    munmap(addr, length);
    close(fd);
    return 0;
}

static UserFriendsEntry *UserFriendsEntryGetUnlock(uint32_t uid)
{
    uint32_t current = uid;
    int reserve[sizeof(current) * 2];
    int end = 0;
    while (current)
    {
        reserve[end++] = current & 0xf;
        current >>= 4;
    }
    --end;
    UserFriendsTable *currentTable = &friendsTable;
    while (end >= 0)
    {
        if (currentTable->next[reserve[end]] == NULL)
        {
            return NULL;
        }
        currentTable = currentTable->next[reserve[end]];
        --end;
    }
    return currentTable->entry;
}

static UserFriendsEntry *UserFriendsEntrySetUnlock(uint32_t uid, UserFriends *friends)
{
    uint32_t current = uid;
    int reserve[sizeof(current) * 2];
    int end = 0;
    while (current)
    {
        reserve[end++] = current & 0xf;
        current >>= 4;
    }
    --end;
    UserFriendsTable *currentTable = &friendsTable;
    while (end >= 0)
    {
        if (currentTable->next[reserve[end]] == NULL)
        {
            currentTable->next[reserve[end]] = calloc(1, sizeof(UserFriendsTable));
        }
        currentTable = currentTable->next[reserve[end]];
        --end;
    }
    UserFriendsEntry *entry = malloc(sizeof(UserFriendsEntry));
    entry->friends = friends;
    pthread_rwlock_init(&entry->lock, NULL);
    pthread_rwlock_init(&entry->refLock, NULL);
    currentTable->entry = entry;
    return entry;
}

/*
static UserFriendsEntry *UserFriendsEntrySet(uint32_t uid, UserFriends *friends)
{
    pthread_rwlock_wrlock(&friendsTableLock);
    UserFriendsEntry *entry = UserFriendsEntrySetUnlock(uid, friends);
    pthread_rwlock_unlock(&friendsTableLock);
    return entry;
}

static UserFriendsEntry *UserFriendsEntryGet(uint32_t uid)
{
    pthread_rwlock_rdlock(&friendsTableLock);
    UserFriendsEntry *entry = UserFriendsEntryGetUnlock(uid);
    pthread_rwlock_unlock(&friendsTableLock);
    return entry;
}
*/
UserFriends *UserFriendsGet(uint32_t uid, pthread_rwlock_t **lock, int access)
{
    UserFriends *friends = NULL;
    void *addr = NULL;
    size_t len = 0;
    int fd = -1;

    pthread_rwlock_wrlock(&friendsTableLock);
    UserFriendsEntry *entry = UserFriendsEntryGetUnlock(uid);
    if (!entry || !entry->friends)
    {
        char path[30];
        UserGetDir(path, uid, "friends");
        fd = open(path, O_RDONLY);
        if (fd == -1)
        {
            log_error("User", "Cannot read user friends file %s.\n", path);
            goto cleanup;
        }
        struct stat statBuf;
        if (fstat(fd, &statBuf))
            goto cleanup;
        len = (size_t) statBuf.st_size;
        addr = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
        if (addr == MAP_FAILED)
        {
            perror("mmap");
            goto cleanup;
        }

        friends = UserFriendsDecode(addr);
        entry = UserFriendsEntrySetUnlock(uid, friends);
    }
    else
    {
        friends = entry->friends;
    }
    pthread_rwlock_rdlock(&entry->refLock);
    switch (access)
    {
        case O_RDONLY:
            pthread_rwlock_rdlock(&entry->lock);
            break;
        case O_WRONLY:
        case O_RDWR:
            pthread_rwlock_wrlock(&entry->lock);
            break;
    }
    if (lock)
        *lock = &entry->lock;

    cleanup:
    pthread_rwlock_unlock(&friendsTableLock);
    if (addr && addr != MAP_FAILED)
        munmap(addr, len);
    if (fd >= 0)
        close(fd);
    return friends;
}

void UserFriendsDrop(uint32_t uid)
{
    pthread_rwlock_wrlock(&friendsTableLock);

    UserFriendsEntry *entry = UserFriendsEntryGetUnlock(uid);
    UserFriends *friends = entry->friends;
    pthread_rwlock_unlock(&entry->lock);
    pthread_rwlock_unlock(&entry->refLock);

    if (pthread_rwlock_trywrlock(&entry->refLock) == 0)
    {
        UserFriendsEntrySetUnlock(uid, NULL);
        pthread_rwlock_unlock(&friendsTableLock);
        UserFriendsSave(uid, friends);
        UserFriendsFree(friends);
        pthread_rwlock_destroy(&entry->lock);
        pthread_rwlock_unlock(&entry->refLock);
        pthread_rwlock_destroy(&entry->refLock);
        free(entry);
        return;
    }
    pthread_rwlock_unlock(&friendsTableLock);
}
