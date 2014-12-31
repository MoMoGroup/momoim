#pragma once

#include <gtk/gtk.h>
#include <imcommon/friends.h>
#include <gtk/gtk.h>
#include "ClientSockfd.h"
#include <protocol/CRPPackets.h>

extern int DeleteEvent();

extern int mysockfd();

extern int maininterface();

extern gboolean mythread(gpointer user_data);

extern gboolean destroyLayout(gpointer user_data);

extern UserFriends *friends;
extern UserGroup *group;

extern int mainchart(friendinfo *friendinfonode);

extern int MessageLoopFunc();

extern void AddMessageNode(uint32_t sessionid, uint16_t packetID, int  (*fn)(CRPBaseHeader *, void *data), void *data);

extern void DeleteMessageNode(uint32_t sessid, uint16_t packetid);

//extern UserFriends *friends;