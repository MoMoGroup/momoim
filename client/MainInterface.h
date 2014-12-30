#pragma once


#include <imcommon/friends.h>
#include <gtk/gtk.h>
#include "ClientSockfd.h"

extern int DeleteEvent();
extern int mysockfd();
extern int maininterface();

extern gboolean mythread(gpointer user_data);

extern gboolean destroyLayout(gpointer user_data);
extern UserFriends *friends;
extern UserGroup *group;
extern int  mainchart(friendinfo *friendinfonode);

extern UserFriends *friends;