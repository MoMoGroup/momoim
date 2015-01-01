#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>

#define DATA_FILE_ROOT_PATH ("files/")
#define DATAFILE_DEPTH_LEVEL 2
extern const uint8_t DataFilePathLength;

int DataFilePath(unsigned char *key, char *buf);

int DataFileExist(unsigned char *key);

int DataFileOpen(unsigned char *key, int flags);

int DataFileCreate(unsigned char *key, int flags, mode_t mode);

int DataFileUnlink(unsigned char *key);

