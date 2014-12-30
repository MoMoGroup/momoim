#pragma once


#include <imcommon/friends.h>
#include <gtk/gtk.h>

extern int DeleteEvent();
extern int mysockfd();
extern int maininterface();

extern gboolean mythread(gpointer user_data);

extern gboolean destroyLayout(gpointer user_data);
extern UserFriends *friends;
extern UserGroup *group;
extern int chardestroyflag;
extern int mainchart();

extern UserFriends *friends;

typedef struct friendinfo {
    uint32_t sessionid;
    int flag;
    //用来判断是否接受成功
    UserInfo user;
    FILE *fp;

    struct friendinfo *next;
} friendinfo;
extern friendinfo *friendinfohead;
