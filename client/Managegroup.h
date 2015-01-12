#pragma once

#include <gdk/gdk.h>
#include <gtk/gtk.h>


//添加
extern int AddGroupButtonPressEvent();

//删除
extern int DeleteGroupButtonPressEvent(GtkWidget *widget, GdkEventButton *event, gpointer data);

//重命名
extern int RenameGroupButtonPressEvent(GtkWidget *widget, GdkEventButton *event, gpointer data);



