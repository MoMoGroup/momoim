#pragma once

#include <gdk/gdk.h>
#include <gtk/gtk.h>


extern int GroupPop(void *data);


//添加
extern int AddGroupButtonPressEvent();

//删除
extern int DeleteGroupButtonPressEvent(GtkWidget *widget, GdkEventButton *event, gpointer data);

//重命名
extern int RenameGroupButtonPressEvent(GtkWidget *widget, GdkEventButton *event, gpointer data);

//上移
extern int UpGroupButtonPressEvent(GtkWidget *widget, GdkEventButton *event, gpointer data);

//上移
extern int DownGroupButtonPressEvent(GtkWidget *widget, GdkEventButton *event, gpointer data);

//移动好友
extern GtkWidget *MovFriendButtonEvent(GtkTreeView *treeview);

extern int ChangeOnLine();

extern int ChangeHideLine();

extern int ShowStatus(void *data);//显示在线隐身的函数







