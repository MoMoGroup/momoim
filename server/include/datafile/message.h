#pragma once

#include "imcommon/message.h"

typedef struct structUserMessageFileEntry {
    MessageFile *file;
    pthread_rwlock_t refLock;
} UserMessageFileEntry;
typedef struct structUserMessageFileTable {
    UserMessageFileEntry *entry;
    struct structUserMessageFileTable *next[0x10];
} UserMessageFileTable;

int UserMessagesInit();

void UserMessagesFinalize();

int UserMessageFileCreate(uint32_t uid);

//MessageFile *UserMessageFileOpen(uint32_t uid);

MessageFile *UserMessageFileGet(uint32_t uid);

void UserMessageFileDrop(uint32_t uid);