#pragma once

#include <imcommon/friends.h>
#include <protocol/info/Data.h>
#include "client.h"

extern int mysockfd();

typedef struct FriendInfo
{
    uint32_t uid;
    int flag;
    //用来判断是否接受成功
    UserInfo user;
    FILE *fp;
    int inonline;
    GtkWidget *chartwindow;
    GtkWidget *chartlayout;
    GtkWidget *chartlayout2;
    GtkWidget *imageflowerbackgroud, *imagesend, *imagehead3, *imagevoice, *imagevideo;
    GtkWidget *imageclose, *imageclosebut;
    GtkWidget *imagelook, *imagejietu, *imagefile, *imagephoto, *imagewordart, *imagecolor;
    GtkWidget *input_text, *show_text;
    GtkScrolledWindow *sw1, *sw2;
    GtkTextBuffer *input_buffer, *show_buffer;

    //查看资料所需控件
    GtkWidget *Infowind;
    GtkWidget *Infolayout, *Infolayout1;
    GtkWidget *Infobackground, *Infocancel, *Infoguanbi, *Infochange;

    struct FriendInfo *next;
} FriendInfo;
extern FriendInfo *FriendInfoHead;
extern CRPContext sockfd;
extern UserInfo *CurrentUserInfo;
extern pthread_t ThreadKeepAlive;
extern int AddFriendflag;//判断是否打开搜索窗口
