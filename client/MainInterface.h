#pragma once

#include <gtk/gtk.h>
#include <imcommon/friends.h>
#include <gtk/gtk.h>
#include "ClientSockfd.h"
#include <protocol/CRPPackets.h>

extern int DeleteEvent();

extern int mysockfd();

extern int MainInterFace();

extern void DestoryMainInterface();

//extern gboolean backttologinLayout(gpointer user_data);

extern gboolean MyThread(gpointer user_data);

extern gboolean DestoryAll(gpointer user_data);

extern gboolean LoadLoginLayout(gpointer user_data);


gboolean DestroyLayout(gpointer user_data);

extern UserFriends *friends;

extern int MainChart(FriendInfo *friendinfonode);

extern void RecdServerMsg(const gchar *rcvd_text, uint16_t len, u_int32_t recd_uid);

extern void RecdServerFileMsg(const gchar *rcvd_text, uint16_t len, u_int32_t recd_uid);
extern int MessageLoopFunc();

extern void AddMessageNode(uint32_t sessionid, int (*fn)(CRPBaseHeader *, void *), void *data);

extern void DeleteMessageNode(uint32_t sessid);
//extern UserFriends *friends;

struct RECVImageMessagedata
{
    int imagecount;
    int charlen;
    char *message_data;
    FriendInfo *userinfo;
};
struct RECVFileMessagedata
{
    GtkWidget *file;
    GtkWidget *progressbar;
    int file_loading_end;
    int file_count;
    int file_size;
    int charlen;
    char *filename;
    FILE *Wfp;
    gchar filemulu[100];
    FriendInfo *userinfo;
};

extern GtkTreeStore *TreeViewListStore;
extern enum {
    PIXBUF_COL = 0,
    FRIENDUID_COL = 1,
    PRIORITY_COL,
} MainInterfaceTreeColumns;

extern GtkWidget *StatusShowText;//显示在线隐身字体

extern int OnLineStatus();

extern GtkWidget *UserId, *HeadX;//修改资料用到的,主界面头像跟昵称

extern int Status(void *data);

extern int MarkNewpasswd;
//标记主界面修改密码按钮的状态
extern int MarkUpdateInfo;//标记主界面头像（修改资料）的状态


