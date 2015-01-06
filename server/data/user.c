#include <fcntl.h>
#include <logger.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <sqlite3.h>
#include <imcommon/friends.h>
#include <data/user.h>

static sqlite3 *db = NULL;
static const char sqlNickInsert[] = "INSERT OR REPLACE INTO info(uid,nick) VALUES(?,?);";
static const char sqlNickQuery[] = "SELECT uid FROM info WHERE nick LIKE ? LIMIT ? OFFSET ?;";
static UserFriendsTable friendsTable;
static pthread_rwlock_t friendsTableLock;

int UserInit()
{
    if (mkdir("user", 0700) != 0 && errno != EEXIST)
    {
        log_error("User", "Cannot create user directory\n");
        return 0;
    }

    int ret;
    ret = sqlite3_open("info.db", &db);
    if (ret != SQLITE_OK)
    {
        return 0;
    }
    pthread_rwlock_init(&friendsTableLock, NULL);
    return 1;
}

void UserFinalize()
{
    pthread_rwlock_destroy(&friendsTableLock);
    sqlite3_close(db);
}

//Get User Directory Path
//18 characters at most
void UserGetDir(char *path, uint32_t uid, const char *relPath)
{
    sprintf(path, "user/%02d/%d/%s", uid % 100, uid / 100, relPath);
}

void UserCreateDirectory(uint32_t uid)
{
    char userDir[20];
    sprintf(userDir, "user/%02d", uid % 100);
    if (mkdir(userDir, 0700) != 0 && errno != EEXIST)
    {
        log_error("User", "Cannot create directory %s\n", userDir);
        return;
    }

    sprintf(userDir, "user/%02d/%d", uid % 100, uid / 100);
    if (mkdir(userDir, 0700) != 0 && errno != EEXIST)
    {
        log_error("User", "Cannot create directory %s\n", userDir);
        return;
    }

    UserFriendsCreate(uid);
}

int UserQueryByNick(const char *text, uint8_t page, uint8_t count, uint32_t *uids)
{
    sqlite3_stmt *stmt;
    if (SQLITE_OK != sqlite3_prepare_v2(db, sqlNickQuery, sizeof(sqlNickQuery), &stmt, NULL))
    {
        return -1;
    }
    sqlite3_bind_text(stmt, 1, text, (int) strlen(text), NULL);
    sqlite3_bind_int(stmt, 2, count);
    sqlite3_bind_int(stmt, 3, (page - 1) * count);
    uint32_t *puid = uids;
    while ((puid - uids < count) && sqlite3_step(stmt) == SQLITE_ROW)
    {
        *puid++ = (uint32_t) sqlite3_column_int(stmt, 1);
    }
    sqlite3_finalize(stmt);
    return (uint8_t) (puid - uids);
}

void UserInfoCreate(uint32_t uid)
{
    UserInfo info = {
            .uid=uid,
            .nickName="Baby",
            .sex=0,
            .icon={
                    [15]=1
            }
    };
    UserInfoSave(uid, &info);
}

int UserInfoSave(uint32_t uid, UserInfo *info)
{
    char path[100];
    UserGetDir(path, uid, "info");

    int fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0600);
    if (fd == -1)
    {
        log_error("User", "Cannot create user info file %s.\n", path);
        return 0;
    }
    write(fd, info, sizeof(UserInfo));
    close(fd);
    sqlite3_stmt *stmt;
    if (SQLITE_OK != sqlite3_prepare_v2(db, sqlNickInsert, sizeof(sqlNickInsert), &stmt, NULL))
    {
        return 0;
    }
    sqlite3_bind_int(stmt, 1, uid);
    sqlite3_bind_text(stmt, 2, info->nickName, (int) strlen(info->nickName), NULL);
    if (SQLITE_DONE != sqlite3_step(stmt))
    {
        sqlite3_finalize(stmt);
        return 0;
    }
    sqlite3_finalize(stmt);
    return 1;
}

UserInfo *UserInfoGet(uint32_t uid)
{
    char infoFile[30];
    UserGetDir(infoFile, uid, "info");
    if (access(infoFile, R_OK))
    {
        UserInfoCreate(uid);
        if (access(infoFile, R_OK))
        {
            return NULL;
        }
    }
    int fd = open(infoFile, O_RDONLY);
    if (fd == -1)
    {
        log_error("User", "Cannot open user info file %s.\n", infoFile);
        return NULL;
    }
    UserInfo *info = (UserInfo *) malloc(sizeof(UserInfo));
    read(fd, info, sizeof(UserInfo));
    close(fd);
    return info;
}

void UserInfoFree(UserInfo *info)
{
    if (info)
        free(info);
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
            .groupCount=2,
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
    UserFriendsTable *currentTable = &friendsTable;
    while (current)
    {
        if (currentTable->next[current & 0xf] == NULL)
        {
            return NULL;
        }
        currentTable = currentTable->next[current & 0xf];
        current >>= 4;
    }
    return currentTable->entry;
}

UserFriendsEntry *UserFriendsEntryGet(uint32_t uid)
{
    pthread_rwlock_rdlock(&friendsTableLock);
    UserFriendsEntry *entry = UserFriendsEntryGetUnlock(uid);
    pthread_rwlock_unlock(&friendsTableLock);
    return entry;
}

static UserFriendsEntry *UserFriendsEntrySetUnlock(uint32_t uid, UserFriends *friends)
{
    uint32_t current = uid;
    UserFriendsTable *currentTable = &friendsTable;
    while (current)
    {
        if (currentTable->next[current & 0xf] == NULL)
        {
            currentTable->next[current & 0xf] = calloc(1, sizeof(UserFriendsTable));
        }
        currentTable = currentTable->next[current & 0xf];
        current >>= 4;
    }
    UserFriendsEntry *entry = malloc(sizeof(UserFriendsEntry));
    entry->friends = friends;
    entry->refCount = 0;
    pthread_rwlock_init(&entry->lock, NULL);
    pthread_mutex_init(&entry->refLock, NULL);
    currentTable->entry = entry;
    return entry;
}

UserFriendsEntry *UserFriendsEntrySet(uint32_t uid, UserFriends *friends)
{
    pthread_rwlock_wrlock(&friendsTableLock);
    UserFriendsEntry *entry = UserFriendsEntrySetUnlock(uid, friends);
    pthread_rwlock_unlock(&friendsTableLock);
    return entry;
}

UserFriends *UserFriendsGet(uint32_t uid, pthread_rwlock_t **lock)
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
    pthread_mutex_lock(&entry->refLock);
    ++entry->refCount;
    pthread_mutex_unlock(&entry->refLock);
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
    pthread_mutex_lock(&entry->refLock);
    --entry->refCount;
    if (entry->refCount <= 0)
    {
        UserFriendsSave(uid, friends);
        UserFriendsFree(friends);
        UserFriendsEntrySetUnlock(uid, NULL);
        pthread_rwlock_destroy(&entry->lock);
        pthread_mutex_unlock(&entry->refLock);
        pthread_mutex_destroy(&entry->refLock);
        free(entry);
    }
    else
    {
        pthread_mutex_unlock(&entry->refLock);
    }
    pthread_rwlock_unlock(&friendsTableLock);
}


int UserMessageFileCreate(uint32_t uid)
{
    char path[30];
    UserGetDir(path, uid, "message");
    return MessageFileCreate(path);
}

MessageFile *UserMessageFileOpen(uint32_t uid)
{
    char path[30];
    UserGetDir(path, uid, "message");
    return MessageFileOpen(path);
}