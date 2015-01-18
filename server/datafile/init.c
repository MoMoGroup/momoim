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
            log_error("Initialization", "无法创建数据目录%s.\n请输入新的路径以存放数据文件(您也可以通过指定程序命令行参数来设置数据目录)\n:", path);
            scanf("%s", path);
            if (*path == 0)
            {
                log_info("Initialization", "操作终止\n");
                exit(1);
            }
            goto createDir;
        }
        else
        {
            log_info("Initialization", "成功创建数据目录%s\n", path);
        }
    }
    log_info("Initialization", "切换当前目录至%s\n", path);
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
                fdWrite = open("files/00/00/0000000000000000000000000001", O_WRONLY);

        if (fdRead < 0)
        {
            log_warning("Initialization", "无法读取默认头像文件(/opt/momo/server/default_icon.png).\n");
            close(fdWrite);
        }
        else if (fdWrite < 0)
        {
            log_error("Initialization", "files目录不可写!\n");
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
