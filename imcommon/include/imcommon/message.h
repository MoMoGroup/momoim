#pragma once

#include <stdint.h>
#include <pthread.h>
//Message Post

typedef enum
{
    UMT_UNKNOW = 0, //未知消息
    UMT_TEXT = 1,//文本消息
} USER_MESSAGE_TYPE;
typedef struct __attribute__ ((packed)) strucUserMessage
{
    uint32_t from, to;
    uint8_t messageType; //USER_MESSAGE_TYPE
    uint16_t messageLen;
    char content[0];
} UserMessage;

typedef struct
{
    size_t count;
    int fd;
    pthread_mutex_t lock;

} UserMessageFile;

int MessageFileCreate(const char *path);

UserMessageFile *MessageFileOpen(const char *path);

int MessageFileCleanup(UserMessageFile *);

int MessageFileClose(UserMessageFile *);

int MessageFileAppend(UserMessageFile *, UserMessage *message);

UserMessage *MessageFileNext(UserMessageFile *);