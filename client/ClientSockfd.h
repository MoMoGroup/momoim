#pragma once

#include <imcommon/friends.h>
#include <protocol/info/Data.h>
#include "client.h"

extern int mysockfd();

typedef struct friendinfo
{
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
    GtkWidget *imagelook, *imagejietu, *imagefile, *imagephoto, *imagewordart;
    GtkWidget *input_text, *show_text;
    GtkScrolledWindow *sw1, *sw2;
    GtkTextBuffer *input_buffer, *show_buffer;
    struct friendinfo *next;
} friendinfo;
extern friendinfo *friendinfohead;
extern CRPContext sockfd;
extern UserInfo CurrentUserInfo;
extern pthread_t ThreadKeepAlive;
