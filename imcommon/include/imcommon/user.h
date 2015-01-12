#pragma once

#include <stdint.h>
#include <time.h>

typedef enum
{
    UOS_ONLINE = 0,
    UOS_HIDDEN
} UserOnlineStatus;
typedef struct __attribute__ ((packed))
{
    uint32_t uid;
    char sex;
    char nickName[32];
    unsigned char icon[16];
    char name[13];
    char birthday[12];
    char constellation;
    //星座
    char provinces[10];
    char city[35];
    char tel[15];
    char school[28];
    char hometown[51];
    time_t lastlogout;
    uint32_t level;
    int postcode;
} UserInfo;