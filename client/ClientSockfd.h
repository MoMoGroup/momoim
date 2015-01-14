#pragma once

#include <imcommon/friends.h>
#include <protocol/info/Data.h>
#include <imcommon/message.h>
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
    GtkWidget *imagelook, *imagejietu, *imagefile, *imagephoto, *imagewordart, *imagecolor, *imagechartrecord;
    GtkWidget *input_text, *show_text;
    GtkScrolledWindow *sw1, *sw2;
    GtkTextBuffer *input_buffer, *show_buffer;
    //聊天记录界面
    GtkWidget *record_window;
    GtkWidget *record_layout;
    GtkWidget *record_layout2;
    GtkWidget *image_record_next, *image_record_close;
    GtkWidget *record_date;
    GtkWidget *record_text;
    GtkWidget *calendar;
    GtkTextBuffer *record_buffer;
    GtkScrolledWindow *record_sw;
    session_id_t session_id;
    int record_end_flag;

    //查看资料所需控件
    GtkWidget *Infowind;
    GtkWidget *Infolayout, *Infolayout1;
    GtkWidget *Infobackground, *Infocancel, *Infoguanbi, *Infochange;
    int FlagInfo;
    struct FriendInfo *next;
} FriendInfo;


extern FriendInfo *FriendInfoHead;
extern CRPContext sockfd;
extern UserInfo *CurrentUserInfo;
extern pthread_t ThreadKeepAlive;
extern int AddFriendflag;//判断是否打开搜索窗口
