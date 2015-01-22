#include <datafile/message.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <logger.h>
#include <sys/user.h>
#include "datafile/init.h"
#include "datafile/auth.h"
#include "datafile/user.h"
#include "datafile/friend.h"

int DataModuleInit()
{
    if (!AuthInit())
    {
        return 0;
    }
    if (!UserInit())
    {
        AuthFinalize();
        return 0;
    }
    if (!UserFriendsInit())
    {
        UserFinalize();
        AuthFinalize();
        return 0;
    }
    if (!UserMessagesInit())
    {
        UserFriendsFinalize();
        UserFinalize();
        AuthFinalize();
        return 0;
    }
    return 1;
}

void DataModuleFinalize()
{
    UserMessagesFinalize();
    UserFriendsFinalize();
    UserFinalize();
    AuthFinalize();
}

void InitServerDataDirectory()
{
#ifndef NDEBUG
    return;
#endif
    char *path = (char *) malloc(PATH_MAX);
    sprintf(path, "%s/momo-server/", getpwuid(getuid())->pw_dir);
    struct stat dirStat;
    createDir:
    if (stat(path, &dirStat) != 0 || !S_ISDIR(dirStat.st_mode))
    {
        if (mkdir(path, 0700))
        {
            log_error("Initialization", "Cannot create directory %s.\nPlease input the data directory\n:", path);
            scanf("%s", path);
            if (*path == 0)
            {
                log_info("Initialization", "Operation Abort\n");
                exit(1);
            }
            goto createDir;
        }
    }
    log_info("Initialization", "Change current directory to %s\n", path);
    chdir(path);
    if (access("temp", F_OK))
    {
        mkdir("temp", 0700);
    }
    if (access("files", F_OK))
    {
        mkdir("files", 0700);
        mkdir("files/00", 0700);
        mkdir("files/00/00", 0700);
        int fdRead = open("/opt/momo-server/default_icon.png", O_RDONLY),
                fdWrite = creat("files/00/00/0000000000000000000000000001",0400);

        if (fdRead < 0)
        {
            log_warning("Initialization", "Cannot read default user icon(/opt/momo/server/default_icon.png).\n");
            close(fdWrite);
        }
        else if (fdWrite < 0)
        {
            log_error("Initialization", "Cannot create file in \"files/\"!\n");
            perror("open");
            close(fdRead);
            exit(1);
        }
        else
        {
            ssize_t n;
            char buffer[PAGE_SIZE];
            while ((n = read(fdRead, buffer, PAGE_SIZE)) > 0)
            {
                write(fdWrite, buffer, n);
            }
            close(fdRead);
            close(fdWrite);
        }
    }
    free(path);
}
