#include <fcntl.h>
#include <logger.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <imcommon/friends.h>

#include "datafile/user.h"

static sqlite3 *db = NULL;
static const char sqlNickInsert[] = "INSERT OR REPLACE INTO info(uid,nick) VALUES(?,?);";
static const char sqlNickQuery[] = "SELECT uid FROM info WHERE nick LIKE ? LIMIT ? OFFSET ?;";

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

    sprintf(userDir + 7, "/%d", uid / 100);
    if (mkdir(userDir, 0700) != 0 && errno != EEXIST)
    {
        log_error("User", "Cannot create directory %s\n", userDir);
        return;
    }

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
            },
            .level=1
    };
    time(&info.lastlogout);
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
    {
        free(info);
    }
}
