#pragma once

#include <gtk/gtk.h>
#include <imcommon/friends.h>
#include <gtk/gtk.h>
#include "ClientSockfd.h"
#include <protocol/CRPPackets.h>

extern int DeleteEvent();

extern int mysockfd();

extern int MainInterFace();

//extern gboolean backttologinLayout(gpointer user_data);

extern gboolean mythread(gpointer user_data);

extern gboolean destoryall(gpointer user_data);

extern gboolean loadloginLayout(gpointer user_data);

extern gboolean DestoryMainInterFace(gpointer user_data);

gboolean DestroyLayout(gpointer user_data);

extern UserFriends *friends;
extern UserGroup *group;

extern int MainChart(friendinfo *friendinfonode);

extern void RecdServerMsg(const gchar *rcvd_text, uint16_t len, u_int32_t recd_uid);

extern int MessageLoopFunc();

extern void AddMessageNode(uint32_t sessionid, int (*fn)(CRPBaseHeader *, void *), void *data);

extern void DeleteMessageNode(uint32_t sessid);

//extern UserFriends *friends;

struct RECvPictureMessageReloadingData
{
    FILE *fp;
    struct RECVImageMessagedata *image_message_data;
};

struct RECVImageMessagedata
{
    int imagecount;
    int charlen;
    char *message_data;
    friendinfo *userinfo;
};