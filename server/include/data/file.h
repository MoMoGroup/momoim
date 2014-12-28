#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>

size_t DataFilePathLength(unsigned char *key);

int DataFilePath(unsigned char *key, char *buf);

int DataFileExist(unsigned char *key);

int DataFileOpen(unsigned char *key, int flags);

int DataFileCreate(unsigned char *key, int flags, mode_t mode);
