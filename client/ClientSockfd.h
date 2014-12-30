#pragma once

#include <imcommon/friends.h>
#include "client.h"
extern int mysockfd();
typedef struct friendinfo {
    uint32_t sessionid;
    int flag;
    //用来判断是否接受成功
    UserInfo user;
    FILE *fp;

    struct friendinfo *next;
} friendinfo;
extern friendinfo *friendinfohead;