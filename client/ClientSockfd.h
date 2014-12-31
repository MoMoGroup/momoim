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
    GtkWidget *chartwindow;
    GtkWidget *chartlayout;
    GtkWidget *chartlayout2;
    GtkWidget *imageflowerbackgroud, *imagesend, *imagehead3, *imagevoice, *imagevideo;
    GtkWidget *imageclose, *imageclosebut;
    struct friendinfo *next;
} friendinfo;
extern friendinfo *friendinfohead;
extern int sockfd;