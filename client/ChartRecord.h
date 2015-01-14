#pragma once

#include "ClientSockfd.h"

extern void ChartRecord(FriendInfo *info);

struct RcordMessageData
{
    int64_t id;
    uint32_t record_user_uid;
    time_t time;
    uint16_t messageLen;
    uint8_t messageType;
    gchar *record_message;
    FriendInfo *info;
};
extern struct RcordMessageData record_message_data;