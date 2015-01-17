#pragma once

#include "ClientSockfd.h"

extern void ChartRecord(FriendInfo *info);

typedef struct RcordMessageData
{
    uint32_t record_user_uid;
    time_t time;
    uint16_t messageLen;
    uint8_t messageType;
    gchar *record_message_char;
    int64_t record_id;

} RcordMessageData;

typedef struct RcordMessage
{
    int i;
    int record_end_flag;
    int imagecount;
    FriendInfo *info;
    int64_t min_id;
    int64_t max_id;
    struct RcordMessageData record_message_data[10];
} RcordMessage;




