#include <stdlib.h>
#include "datafile/message.h"
#include "datafile/user.h"


static UserMessageFileTable userMessagesTable;
static pthread_mutex_t userMessageTableLock;


int UserMessagesInit()
{
    pthread_mutex_init(&userMessageTableLock, NULL);
    return 1;
}

void UserMessagesFinalize()
{
    pthread_mutex_destroy(&userMessageTableLock);
}

int UserMessageFileCreate(uint32_t uid)
{
    char path[30];
    UserGetDir(path, uid, "message");
    return MessageFileCreate(path);
}

MessageFile *UserMessageFileOpen(uint32_t uid)
{
    char path[30];
    UserGetDir(path, uid, "message");
    return MessageFileOpen(path);
}

static UserMessageFileEntry *UserMessageEntryGetUnlock(uint32_t uid)
{
    uint32_t current = uid;
    int reserve[sizeof(current) * 2];
    int end = 0;
    while (current)
    {
        reserve[end++] = current & 0xf;
        current >>= 4;
    }
    --end;
    UserMessageFileTable *currentTable = &userMessagesTable;
    while (end >= 0)
    {
        if (currentTable->next[reserve[end]] == NULL)
        {
            return NULL;
        }
        currentTable = currentTable->next[reserve[end]];
        --end;
    }
    return currentTable->entry;
}

static UserMessageFileEntry *UserMessageTableSetUnlock(uint32_t uid, MessageFile *file)
{
    uint32_t current = uid;
    int reserve[sizeof(current) * 2];
    int end = 0;
    while (current)
    {
        reserve[end++] = current & 0xf;
        current >>= 4;
    }
    --end;
    UserMessageFileTable *currentTable = &userMessagesTable;
    while (end >= 0)
    {
        if (currentTable->next[reserve[end]] == NULL)
        {
            currentTable->next[reserve[end]] = calloc(1, sizeof(UserMessageFileTable));
        }
        currentTable = currentTable->next[reserve[end]];
        --end;
    }
    UserMessageFileEntry *entry = (UserMessageFileEntry *) malloc(sizeof(UserMessageFileEntry));
    entry->file = file;
    pthread_rwlock_init(&entry->refLock, NULL);
    currentTable->entry = entry;
    return entry;
}

MessageFile *UserMessageFileGet(uint32_t uid)
{
    UserMessageFileEntry *entry;
    pthread_mutex_lock(&userMessageTableLock);
    entry = UserMessageEntryGetUnlock(uid);
    if (!entry)
    {
        MessageFile *file = UserMessageFileOpen(uid);
        entry = UserMessageTableSetUnlock(uid, file);
    }
    pthread_rwlock_rdlock(&entry->refLock);
    pthread_mutex_unlock(&userMessageTableLock);
    return entry->file;
}

void UserMessageFileDrop(uint32_t uid)
{
    UserMessageFileEntry *entry;
    pthread_mutex_lock(&userMessageTableLock);
    entry = UserMessageEntryGetUnlock(uid);
    pthread_rwlock_unlock(&entry->refLock);

    if (pthread_rwlock_trywrlock(&entry->refLock) == 0)
    {
        UserMessageTableSetUnlock(uid, NULL);
        pthread_mutex_unlock(&userMessageTableLock);
        MessageFileClose(entry->file);
    }
    pthread_mutex_unlock(&userMessageTableLock);
}
