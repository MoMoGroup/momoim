#pragma once

#include <stdint.h>
#include <pthread.h>
#include <fcntl.h>
//Message Post

typedef enum {
    UMT_UNKNOW = 0, //未知消息
    UMT_TEXT = 1,   //文本消息
    UMT_NEW_FRIEND,  //添加好友请求
    UMT_FRIEND_ACCEPT
} USER_MESSAGE_TYPE;
typedef struct __attribute__ ((packed)) strucUserMessage {
    uint32_t from, to;
    time_t time;
    uint8_t messageType; //USER_MESSAGE_TYPE
    uint16_t messageLen;
    char content[0];
} UserMessage;

typedef struct {
    size_t count;
    int fd;
    uint32_t fileBeginDate, lastUpdateDate, currentDate;
    off_t fileBeginOffset, currentBeginOffset;
    pthread_mutex_t lock;
} MessageFile;

extern int MessageFileCreate(const char *path);

extern MessageFile *MessageFileOpen(const char *path);

extern int MessageFileClose(MessageFile *);

extern int MessageFileAppend(MessageFile *, UserMessage *message);

extern int MessageFileSeek(MessageFile *, uint32_t date);

extern UserMessage *MessageFileNext(MessageFile *);
