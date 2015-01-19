#include <fcntl.h>
#include <unistd.h>
#include <sys/user.h>
#include <dirent.h>
#include <stdlib.h>
#include <search.h>
#include "imcommon/fileutil.h"

ssize_t FileCopy(const char *src, const char *dst)
{
    ssize_t totalSize = 0, size;
    int fdSrc = open(src, O_RDONLY);
    if (fdSrc < 0)
    {
        return -1;
    }
    int fdDst = open(dst, O_WRONLY);
    if (fdDst < 0)
    {
        close(fdSrc);
        return -1;
    }
    char buffer[PAGE_SIZE];
    while ((size = read(fdSrc, buffer, PAGE_SIZE)) > 0)
    {
        totalSize += size;
        write(fdDst, buffer, (size_t) size);
    }
    close(fdSrc);
    close(fdDst);
    return totalSize;
}
