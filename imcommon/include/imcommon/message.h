#pragma once

#include <stdint.h>
#include <pthread.h>
#include <fcntl.h>
#include "sqlite3.h"
//Message Post

typedef enum
{
    UMT_UNKNOW = 0, //未知消息(数据错误)
    UMT_TEXT = 1,   //文本消息
    UMT_NEW_FRIEND,  //添加好友请求
    UMT_FRIEND_ACCEPT,
    UMT_FILE_OFFLINE,
    UMT_FILE_ONLINE,
} USER_MESSAGE_TYPE;
typedef struct __attribute__ ((packed)) strucUserMessage
{
    int64_t id;
    uint32_t from, to;
    time_t time;
    uint8_t messageType; //USER_MESSAGE_TYPE
    uint16_t messageLen;
    char content[0];
} UserMessage;

typedef struct
{
    sqlite3 *db;
    sqlite3_mutex *mutex;
} MessageFile;
typedef struct
{
    uint64_t id;           //-1表示不筛选
    time_t time;           //-1表示不筛选
    int idOperator;        //-2:<,-1:<=,0:=,1:>=,2:>
    int timeOperator;      //-2:<,-1:<=,0:=,1:>=,2:>

    uint32_t from, to;     //0表示不筛选
    int fromtoOperator;    //3:AND,其他:OR
    uint8_t messageType;   //255表示不筛选
    uint8_t limit;         //传入0将被替换为20

} MessageQueryCondition;

extern int MessageFileCreate(const char *path);

extern MessageFile *MessageFileOpen(const char *path);

extern int MessageFileClose(MessageFile *);

extern int64_t MessageFileInsert(MessageFile *, UserMessage *message);

extern UserMessage **MessageFileQuery(MessageFile *file, MessageQueryCondition *condition, int *count);