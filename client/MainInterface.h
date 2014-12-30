#pragma once


#include <imcommon/friends.h>
#include <gtk/gtk.h>

extern int DeleteEvent();
extern int mysockfd();
extern int maininterface();

extern gboolean mythread(gpointer user_data);

extern gboolean destroyLayout(gpointer user_data);
extern UserFriends *friends;
extern UserGroup *group;
extern int chardestroyflag;
extern int mainchart();

extern UserFriends *friends;