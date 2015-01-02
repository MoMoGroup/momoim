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

#include "run/user.h"
#include "data/user.h"

static sqlite3 *db = NULL;
static const char sqlNickInsert[] = "INSERT OR REPLACE INTO info(uid,nick) VALUES(?,?);";
static const char sqlNickQuery[] = "SELECT FROM info WHERE nick LIKE ? LIMIT ? OFFSET ?;";

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
    return 1;
}

void UserFinalize()
{
    sqlite3_close(db);
}

//Get User Directory Path
//17 characters at least
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

    UserInfoCreate(uid);
    UserCreateFriendsFile(uid);
}

int UserQueryByNick(const char *text, uint page, uint count, uid_t *uids)
{
    //TODO BREAK HERE
    sqlite3_stmt *stmt;
    if (SQLITE_OK != sqlite3_prepare_v2(db, sqlNickQuery, sizeof(sqlNickQuery), &stmt, NULL))
    {
        return 0;
    }
    sqlite3_bind_text(stmt, 1, text, (int) strlen(text), NULL);
    sqlite3_bind_int(stmt, 2, count);
    sqlite3_bind_int(stmt, 3, (page - 1) * count);
    if (SQLITE_DONE != sqlite3_step(stmt))
    {
        sqlite3_finalize(stmt);
        return 0;
    }
    sqlite3_finalize(stmt);

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

void UserInfoFree(UserInfo *friends)
{
    if (friends)
        free(friends);
}

void UserCreateFriendsFile(uint32_t uid)
{
    uint32_t mfriends[] = {
            uid
    };
    UserGroup groups[] = {
            {
                    .groupId=0,
                    .groupName="Friends",
                    .friendCount=1,
                    .friends=mfriends
            },
            {
                    .groupId=UINT8_MAX,
                    .groupName="Blacklist",
                    .friendCount=0
            }
    };
    UserFriends friends = {
            .groupCount=2,
            .groups=groups
    };
    UserSaveFriendsFile(uid, &friends);
};

int UserSaveFriendsFile(uint32_t uid, UserFriends *friends)
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

UserFriends *UserGetFriends(uint32_t uid)
{
    UserFriends *friends = NULL;
    char path[30];
    UserGetDir(path, uid, "friends");
    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        log_error("User", "Cannot read user friends file %s.\n", path);
        return NULL;
    }
    //Map File To Memory
    struct stat statBuf;
    if (fstat(fd, &statBuf))
        goto cleanup;
    size_t len = (size_t) statBuf.st_size;
    void *addr = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED)
        goto cleanup;

    friends = UserFriendsDecode(addr);

    cleanup:
    if (addr && addr != MAP_FAILED)
        munmap(addr, len);
    close(fd);
    return friends;
}

void UserFreeFriends(UserFriends *friends)
{
    UserFriendsFree(friends);
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