#pragma once

#include <gtk/gtk.h>
#include <imcommon/friends.h>

extern int DeleteEvent();
extern int mysockfd();
extern int maininterface();

extern gboolean mythread(gpointer user_data);

extern gboolean destroyLayout(gpointer user_data);
extern UserFriends *friends;
extern UserGroup *group;
extern char mulu[80];

extern int mainchart();

extern UserFriends *friends;