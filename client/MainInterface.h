#pragma once

#include <gtk/gtk.h>
#include <imcommon/friends.h>
#include <gtk/gtk.h>
#include "ClientSockfd.h"
#include <protocol/CRPPackets.h>

extern int DeleteEvent();

extern int mysockfd();

extern int maininterface();

//extern gboolean backttologinLayout(gpointer user_data);

extern gboolean mythread(gpointer user_data);

extern gboolean destoryall(gpointer user_data);

extern gboolean loadloginLayout(gpointer user_data);

extern gboolean DestoryMainInterFace(gpointer user_data);

gboolean destroyLayout(gpointer user_data);

extern UserFriends *friends;
extern UserGroup *group;

extern int mainchart(friendinfo *friendinfonode);

extern void recd_server_msg(const gchar *rcvd_text, u_int32_t recd_uid);

extern int MessageLoopFunc();

extern void AddMessageNode(uint32_t sessionid, uint16_t packetID, int  (*fn)(CRPBaseHeader *, void *data), void *data);

extern void DeleteMessageNode(uint32_t sessid, uint16_t packetid);

//extern UserFriends *friends;