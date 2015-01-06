#pragma once

#include <stdint.h>
#include "imcommon/message.h"
#include "imcommon/friends.h"

typedef struct structUserMessagesTable
{
    MessageFile *file;
    struct structUserMessagesTable *next[0x10];
} UserMessagesTable;


int UserInit();

void UserFinalize();

void UserGetDir(char *path, uint32_t uid, const char *relPath);

void UserCreateDirectory(uint32_t uid);

int UserQueryByNick(const char *text, uint8_t page, uint8_t count, uint32_t *uids);

//Info
void UserInfoCreate(uint32_t uid);

int UserInfoSave(uint32_t uid, UserInfo *info);

UserInfo *UserInfoGet(uint32_t uid);

void UserInfoFree(UserInfo *info);


int UserMessageFileCreate(uint32_t uid);

MessageFile *UserMessageFileOpen(uint32_t uid);