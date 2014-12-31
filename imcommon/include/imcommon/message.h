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

} MessageFile;

int MessageFileCreate(const char *path);

MessageFile *MessageFileOpen(const char *path);

int MessageFileCleanup(MessageFile *);

int MessageFileClose(MessageFile *);

int MessageFileAppend(MessageFile *, UserMessage *message);

UserMessage *MessageFileNext(MessageFile *);