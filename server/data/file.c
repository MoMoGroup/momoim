#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <logger.h>
#include "data/file.h"

const uint8_t FileDepthLevel = 2;
#define DATA_FILE_ROOT_PATH ("files/")

size_t DataFilePathLength(unsigned char *key)
{
    return 6 +    //"files/"
           FileDepthLevel * 3 +//深度字符串
           (16 - FileDepthLevel) * 2 + 1;//剩余十六进制字符
}

int DataFilePath(unsigned char *key, char *buf)
{
    memcpy(buf, DATA_FILE_ROOT_PATH, sizeof(DATA_FILE_ROOT_PATH) - 1);
    char *p = buf + sizeof(DATA_FILE_ROOT_PATH) - 1;
    int i;
    for (i = 0; i < 16; ++i)
    {
        *p++ = (char) ((key[i] >> 4) > 9 ? 'a' + ((key[i] >> 4) - 10) : '0' + (key[i] >> 4));
        *p++ = (char) ((key[i] & 0xf) > 9 ? 'a' + ((key[i] & 0xf) - 10) : '0' + (key[i] & 0xf));
        if (i < FileDepthLevel)
        {
            *p++ = '/';
        }
    }
    *p = 0;
    return 1;
}

int DataFileExist(unsigned char *key)
{
    char buf[6 + FileDepthLevel * 3 + (16 - FileDepthLevel) * 2 + 1];
    DataFilePath(key, buf);
    return !access(buf, F_OK);
}

int DataFileOpen(unsigned char *key, int flags)
{
    char buf[6 + FileDepthLevel * 3 + (16 - FileDepthLevel) * 2 + 1];
    DataFilePath(key, buf);
    return open(buf, flags);
}

int DataFileCreate(unsigned char *key, int flags, mode_t mode)
{
    char buf[6 + FileDepthLevel * 3 + (16 - FileDepthLevel) * 2 + 1];
    DataFilePath(key, buf);
    return open(buf, flags, mode);
}