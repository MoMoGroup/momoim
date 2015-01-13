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

extern gboolean mythread(gpointer user_data);

extern gboolean destoryall(gpointer user_data);

extern gboolean loadloginLayout(gpointer user_data);


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
    FriendInfo *userinfo;
};

extern GtkTreeStore *TreeViewListStore;
extern enum {
    PIXBUF_COL = 0,
    FRIENDUID_COL = 1,
    PRIORITY_COL,
};

extern GtkWidget *userid, *headx;
