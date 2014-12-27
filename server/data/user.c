#include <fcntl.h>
#include <user.h>
#include <logger.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include "data/user.h"

//Get User Directory Path
//17 characters at least
void UserGetDir(char *path, uint32_t uid, const char *relPath)
{
    sprintf(path, "user/%02d/%d/%s", uid % 100, uid / 100, relPath);
}

void UserCreateDirectory(uint32_t uid)
{
    char userDir[20], path[100];
    sprintf(userDir, "user");
    if (mkdir(userDir, 0700) != 0 && errno != EEXIST)
    {
        log_error("User", "Cannot create directory %s\n", userDir);
        return;
    }
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

    sprintf(path, "%s/info", userDir);
    UserCreateInfoFile(uid, path);

    sprintf(path, "%s/group", userDir);
    UserCreateFriendsFile(path);
}

void UserCreateInfoFile(uint32_t uid, char *path)
{
    UserInfo info = {
            .uid=uid,
            .nickName="Baby",
            .sex=0
    };
    int fd = open(path, O_CREAT | O_WRONLY, 0600);
    if (fd == -1)
    {
        log_error("User", "Cannot create user info file %s.\n", path);
        return;
    }
    write(fd, &info, sizeof(info));
    close(fd);
}

UserInfo *UserGetInfo(uint32_t uid)
{
    char infoFile[30];
    UserGetDir(infoFile, uid, "info");
    if (access(infoFile, R_OK))
    {
        UserCreateInfoFile(uid, infoFile);
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

void UserCreateFriendsFile(char *path)
{

}