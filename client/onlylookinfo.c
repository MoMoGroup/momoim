#include <gtk/gtk.h>
#include <imcommon/friends.h>
#include <string.h>
#include <stdlib.h>
#include <imcommon/user.h>
#include <math.h>
#include "ClientSockfd.h"
#include "common.h"
#include "Infomation.h"
#include "MainInterface.h"

static cairo_surface_t *Surfaceback, *Surfacecancel, *Surfacecancel1, *Surfaceend, *Surfaceend1, *Surfaceend2, *Surfacechange, *Surfacechange1;
static cairo_surface_t *surfacehead;

static const char *constellations[] = {
        "水瓶座", "双鱼座", "白羊座", "金牛座",
        "双子座", "巨蟹座", "狮子座", "处女座",
        "天秤座", "天蝎座", "射手座", "摩羯座"
};

static void create_infofaces()
{
        Surfaceback = ChangeThem_png("资料.png");
        Surfacecancel = ChangeThem_png("关闭1.png");
        Surfacecancel1 = ChangeThem_png("关闭2.png");
        Surfaceend = ChangeThem_png("关闭按钮1.png");
        Surfaceend1 = ChangeThem_png("关闭按钮2.png");
        Surfaceend2 = ChangeThem_png("关闭按钮3.png");
        Surfacechange = ChangeThem_png("更新.png");
        Surfacechange1 = ChangeThem_png("更新2.png");
}



//背景的eventbox拖曳窗口
static gint Infobackg_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   event->x_root, event->y_root, event->time);
    }
    return 0;
}

//关闭按钮
//鼠标点击事件
static gint cancel_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)
    {
        gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//关闭按钮
//鼠标抬起事件
static gint cancel_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)
    {
        gtk_widget_destroy(info->Infowind);
        info->Infowind = NULL;
        MarkUpdateInfo = 0;
    }
    return 0;
}

//关闭按钮
//鼠标移动事件
static gint cancel_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) info->Infocancel, Surfacecancel1); //置换图标
    return 0;
}

//关闭按钮
//鼠标离开事件
static gint cancel_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->Infocancel, Surfacecancel);

    return 0;
}

//关闭
//鼠标点击事件
static gint guanxx_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)
    {
        gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->Infoguanbi, Surfaceend1); //置换图标
    }
    return 0;
}

//关闭
//鼠标抬起事件
static gint guanxx_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)
    {
        gtk_widget_destroy(info->Infowind);
        info->Infowind = NULL;
        MarkUpdateInfo = 0;
    }
    return 0;
}

//关闭
//鼠标移动事件
static gint guanxx_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) info->Infoguanbi, Surfaceend2);
    return 0;
}

//关闭
//鼠标离开事件
static gint guanxx_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->Infoguanbi, Surfaceend);
    return 0;
}

//更新
//鼠标点击事件
static gint change_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)
    {
        gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//更新
//鼠标抬起事件
static gint change_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)
    {
        ChangeInfo();
        gtk_widget_destroy(info->Infowind);
        info->Infowind = NULL;
        MarkUpdateInfo = 1;
    }
    return 0;
}

//更新
//鼠标移动事件
static gint change_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) info->Infochange, Surfacechange1); //置换图标
    return 0;
}

//更新
//鼠标离开事件
static gint change_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->Infochange, Surfacechange);
    return 0;
}

int OnlyLookInfo(FriendInfo *friendinfonode)
{

    GtkEventBox *Infobackg_event_box, *Cancel_event_box, *Guanxx_event_box, *Change_event_box;

    friendinfonode->Infowind = gtk_window_new(GTK_WINDOW_TOPLEVEL);//创建新窗口
    gtk_window_set_position(GTK_WINDOW(friendinfonode->Infowind), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (friendinfonode->Infowind), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(friendinfonode->Infowind), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(friendinfonode->Infowind), 550, 488);//设置窗口大小

    friendinfonode->Infolayout = gtk_fixed_new();//创建布局容纳控件
    friendinfonode->Infolayout1 = gtk_layout_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(friendinfonode->Infowind), friendinfonode->Infolayout1);
    gtk_container_add(GTK_CONTAINER (friendinfonode->Infolayout1), friendinfonode->Infolayout);

    create_infofaces();

    friendinfonode->Infobackground = gtk_image_new_from_surface(Surfaceback);
    friendinfonode->Infocancel = gtk_image_new_from_surface(Surfacecancel);
    friendinfonode->Infoguanbi = gtk_image_new_from_surface(Surfaceend);
    friendinfonode->Infochange = gtk_image_new_from_surface(Surfacechange);

    Infobackg_event_box = BuildEventBox(friendinfonode->Infobackground,
                                        G_CALLBACK(Infobackg_button_press_event),
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        friendinfonode);
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), (GtkWidget *) Infobackg_event_box, 0, 0);

    Cancel_event_box = BuildEventBox(friendinfonode->Infocancel,
                                     G_CALLBACK(cancel_button_press_event),
                                     G_CALLBACK(cancel_enter_notify_event),
                                     G_CALLBACK(cancel_leave_notify_event),
                                     G_CALLBACK(cancel_button_release_event),
                                     NULL,
                                     friendinfonode);
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), (GtkWidget *) Cancel_event_box, 450, 440);

    Guanxx_event_box = BuildEventBox(friendinfonode->Infoguanbi,
                                     G_CALLBACK(guanxx_button_press_event),
                                     G_CALLBACK(guanxx_enter_notify_event),
                                     G_CALLBACK(guanxx_leave_notify_event),
                                     G_CALLBACK(guanxx_button_release_event),
                                     NULL,
                                     friendinfonode);
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), (GtkWidget *) Guanxx_event_box, 509, 0);

    if (CurrentUserInfo->uid == friendinfonode->user.uid)
    {
        Change_event_box = BuildEventBox(friendinfonode->Infochange,
                                         G_CALLBACK(change_button_press_event),
                                         G_CALLBACK(change_enter_notify_event),
                                         G_CALLBACK(change_leave_notify_event),
                                         G_CALLBACK(change_button_release_event),
                                         NULL,
                                         friendinfonode);
        gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), (GtkWidget *) Change_event_box, 350, 440);
    }

    GtkWidget *iid, *ilevel, *isex, *inickname, *iname, *ibirthday, *iconstellation, *iprovinces, *icity;
    GtkWidget *itel, *imail;
    GtkWidget *headicon, *imotto;

    char idstring[80] = {0};
    sprintf(idstring, "%d", friendinfonode->user.uid);
    iid = gtk_label_new(idstring);//id
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), iid, 255, 29);

    memset(idstring, 0, strlen(idstring));
    sprintf(idstring, "%d", friendinfonode->user.level);
    ilevel = gtk_label_new(idstring);//等级
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), ilevel, 270, 70);

    inickname = gtk_label_new(friendinfonode->user.nickName);//昵称
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), inickname, 58, 180);

    iname = gtk_label_new(friendinfonode->user.name);//姓名
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), iname, 48, 241);

    imail = gtk_label_new(friendinfonode->user.mail);//邮箱
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), imail, 305, 353);

    imotto = gtk_label_new(friendinfonode->user.motto);//个人说明
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), imotto, 75, 383);

    itel = gtk_label_new(friendinfonode->user.tel);//电话
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), itel, 48, 353);

    iprovinces = gtk_label_new(friendinfonode->user.provinces);//省份
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), iprovinces, 48, 295);

    icity = gtk_label_new(friendinfonode->user.city);//城市
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), icity, 305, 295);

    ibirthday = gtk_label_new(friendinfonode->user.birthday);//生日
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), ibirthday, 48, 270);

    if (0 == friendinfonode->user.sex)
    {//性别
        isex = gtk_label_new("女");
    }
    else
    {
        isex = gtk_label_new("男");
    }
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), isex, 305, 240);

    iconstellation = gtk_label_new("摩羯座");
    for (int i = 0; i < 12; ++i)
    {
        if (friendinfonode->user.constellation == i)
        {
            iconstellation = gtk_label_new(constellations[i]);//星座
            break;
        }
    }
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), iconstellation, 305, 266);

    char infohead[80] = {0};
    static cairo_t *cr;
    cairo_surface_t *surface;
    HexadecimalConversion(infohead, friendinfonode->user.icon);
    //加载一个图片
    surface = cairo_image_surface_create_from_png(infohead);
    int w = cairo_image_surface_get_width(surface);
    int h = cairo_image_surface_get_height(surface);
    //创建画布
    surfacehead = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 125, 125);
    //创建画笔
    cr = cairo_create(surfacehead);
    //缩放
    cairo_arc(cr, 60, 60, 60, 0, M_PI * 2);
    cairo_clip(cr);
    cairo_scale(cr, 125.0 / w, 126.0 / h);
    //把画笔和图片相结合。
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
    headicon = gtk_image_new_from_surface(surfacehead);
    cairo_destroy(cr);
    gtk_fixed_put(GTK_FIXED(friendinfonode->Infolayout), headicon, 25, 15);

    gtk_widget_show_all(friendinfonode->Infowind);
    return 0;
}