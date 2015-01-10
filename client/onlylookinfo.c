#include <gtk/gtk.h>
#include <imcommon/friends.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include "ClientSockfd.h"
#include "common.h"

static cairo_surface_t *Surfaceback, *Surfacecancel, *Surfacecancel1, *Surfaceend, *Surfaceend1, *Surfaceend2, *Surfacechange, *Surfacechange1;
static cairo_surface_t *surfacehead;

static void create_infofaces(FriendInfo *information) {
    if (Surfaceback == NULL) {
        Surfaceback = cairo_image_surface_create_from_png("资料.png");
        Surfacecancel = cairo_image_surface_create_from_png("关闭1.png");
        Surfacecancel1 = cairo_image_surface_create_from_png("关闭2.png");
        Surfaceend = cairo_image_surface_create_from_png("关闭按钮1.png");
        Surfaceend1 = cairo_image_surface_create_from_png("关闭按钮2.png");
        Surfaceend2 = cairo_image_surface_create_from_png("关闭按钮3.png");
        Surfacechange = cairo_image_surface_create_from_png("更新.png");
        Surfacechange1 = cairo_image_surface_create_from_png("更新2.png");
    }
}

//背景的eventbox拖曳窗口
static gint Infobackg_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1) {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   event->x_root, event->y_root, event->time);
    }
    return 0;
}

//关闭按钮
//鼠标点击事件
static gint cancel_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1) {
        gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//关闭按钮
//鼠标抬起事件
static gint cancel_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1) {
        gtk_widget_destroy(info->Infowind);
        info->Infowind = NULL;
    }
    return 0;
}

//关闭按钮
//鼠标移动事件
static gint cancel_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) info->Infocancel, Surfacecancel1); //置换图标
    return 0;
}

//关闭按钮
//鼠标离开事件
static gint cancel_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->Infocancel, Surfacecancel);

    return 0;
}

//关闭
//鼠标点击事件
static gint guanxx_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1) {
        gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->Infoguanbi, Surfaceend1); //置换图标
    }
    return 0;
}

//关闭
//鼠标抬起事件
static gint guanxx_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1) {
        gtk_widget_destroy(info->Infowind);
        info->Infowind = NULL;
    }
    return 0;
}

//关闭
//鼠标移动事件
static gint guanxx_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) info->Infoguanbi, Surfaceend2);
    return 0;
}

//关闭
//鼠标离开事件
static gint guanxx_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->Infoguanbi, Surfaceend);
    return 0;
}

//更新
//鼠标点击事件
static gint save_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1) {
        gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//更新
//鼠标抬起事件
static gint save_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1) {
        //infosockfd();
    }
    return 0;
}

//更新
//鼠标移动事件
static gint save_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) info->Infochange, Surfacechange1); //置换图标
    return 0;
}

//更新
//鼠标离开事件
static gint save_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->Infochange, Surfacechange);
    return 0;
}

int OnlyLookInfo(FriendInfo *friendinfonode) {

    GtkEventBox *Infobackg_event_box, *Cancel_event_box, *Guanxx_event_box, *Change_event_box;

    friendinfonode->Infowind = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(friendinfonode->Infowind), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (friendinfonode->Infowind), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(friendinfonode->Infowind), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(friendinfonode->Infowind), 550, 488);

    friendinfonode->Infolayout = gtk_fixed_new();
    friendinfonode->Infolayout1 = gtk_layout_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(friendinfonode->Infowind), friendinfonode->Infolayout1);
    gtk_container_add(GTK_CONTAINER (friendinfonode->Infolayout1), friendinfonode->Infolayout);

    create_infofaces(friendinfonode);

    friendinfonode->Infobackground = gtk_image_new_from_surface(Surfaceback);
    friendinfonode->Infocancel = gtk_image_new_from_surface(Surfacecancel);
    friendinfonode->Infoguanbi = gtk_image_new_from_surface(Surfaceend);

    Infobackg_event_box = BuildEventBox(friendinfonode->Infobackground,
                                        G_CALLBACK(Infobackg_button_press_event),
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        friendinfonode);
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), Infobackg_event_box, 0, 0);

    Cancel_event_box = BuildEventBox(friendinfonode->Infocancel,
                                     G_CALLBACK(cancel_button_press_event),
                                     G_CALLBACK(cancel_enter_notify_event),
                                     G_CALLBACK(cancel_leave_notify_event),
                                     G_CALLBACK(cancel_button_release_event),
                                     NULL,
                                     friendinfonode);
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), Cancel_event_box, 450, 440);

    Guanxx_event_box = BuildEventBox(friendinfonode->Infoguanbi,
                                     G_CALLBACK(guanxx_button_press_event),
                                     G_CALLBACK(guanxx_enter_notify_event),
                                     G_CALLBACK(guanxx_leave_notify_event),
                                     G_CALLBACK(guanxx_button_release_event),
                                     NULL,
                                     friendinfonode);
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), Guanxx_event_box, 509, 0);

    if (CurrentUserInfo.uid == friendinfonode->user.uid) {
        Change_event_box = BuildEventBox(friendinfonode->Infochange,
                                         G_CALLBACK(save_button_press_event),
                                         G_CALLBACK(save_enter_notify_event),
                                         G_CALLBACK(save_leave_notify_event),
                                         G_CALLBACK(save_button_release_event),
                                         NULL,
                                         NULL);
        gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), Change_event_box, 350, 440);
    }

    GtkWidget *iid, *ilevel, *isex, *inickname, *iname, *ibirthday, *iconstellation, *iprovinces, *icity;
    GtkWidget *itel, *ischool, *ipostcode, *ihometown;
    GtkWidget *headicon;

    char idstring[80] = {0};
    sprintf(idstring, "%d", friendinfonode->user.uid);
    iid = gtk_label_new(idstring);//id
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), iid, 240, 30);

    memset(idstring, 0, strlen(idstring));
    sprintf(idstring, "等级：%d", friendinfonode->user.level);
    ilevel = gtk_label_new(idstring);//等级
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), ilevel, 200, 50);

    inickname = gtk_label_new(friendinfonode->user.nickName);//昵称
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), inickname, 58, 180);

    iname = gtk_label_new(friendinfonode->user.name);//姓名
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), iname, 48, 243);

    ipostcode = gtk_label_new(friendinfonode->user.postcode);//邮编
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), ipostcode, 305, 353);

    ischool = gtk_label_new(friendinfonode->user.school);//毕业院校
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), ischool, 75, 383);

    ihometown = gtk_label_new(friendinfonode->user.hometown);//故乡
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), ihometown, 48, 410);

    itel = gtk_label_new(friendinfonode->user.tel);//电话
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), itel, 48, 355);

    iprovinces = gtk_label_new(friendinfonode->user.provinces);//省份
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), iprovinces, 48, 295);

    icity = gtk_label_new(friendinfonode->user.city);//城市
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), icity, 305, 295);

    ibirthday = gtk_label_new(friendinfonode->user.birthday);//生日
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), ibirthday, 48, 270);

    if (0 == friendinfonode->user.sex)//性别
        isex = gtk_label_new("女");
    else
        isex = gtk_label_new("男");
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), isex, 305, 240);

    for (int i = 0; i < 12; ++i) {
        if (CurrentUserInfo.constellation == i) {
            iconstellation = gtk_label_new(friendinfonode->user.name);//星座
            break;
        }
    }
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), iconstellation, 305, 268);

    char infohead[80] = {0};
    sprintf(infohead, "%s/.momo/friend/%d.png", getpwuid(getuid())->pw_dir, friendinfonode->user.uid);
    surfacehead = cairo_image_surface_create_from_png(infohead);
    headicon = gtk_image_new_from_surface(surfacehead);
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), headicon, 23, 16);

    gtk_widget_show_all(friendinfonode->Infowind);

    return 0;
}