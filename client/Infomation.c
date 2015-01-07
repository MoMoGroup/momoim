#include <gtk/gtk.h>
#include <imcommon/friends.h>
#include <pwd.h>
#include "ClientSockfd.h"
#include "common.h"

static GtkWidget *Infowind;
static GtkWidget *Infolayout;
static GtkEventBox *Infobackg_event_box, *Save_event_box, *Cancel_event_box, *Guanxx_event_box;
static GtkWidget *Infobackground, *Infosave, *Infocancel, *Infoguanbi;
static cairo_surface_t *Surfaceback, *Surfacesave, *Surfacesave1, *Surfacecancel, *Surfacecancel1, *Surfaceend, *Surfaceend1, *Surfaceend2;

static GtkWidget *iid, *isex, *inickName, *inote, *iname, *ibloodtype, *ibirthday, *iconstellation, *izodiac, *iprovinces, *icity, *icounty;
static GtkWidget *iphonenumber, *itel, *ischoolrecord, *iprofessional, *ischool, *ihometown, *ihome;
static cairo_surface_t *surfacehead;
static GtkWidget *headicon;
static GtkEventBox *Inote_event_box;

static void create_infofaces() {

    Surfaceback = cairo_image_surface_create_from_png("资料.png");
    Surfacesave = cairo_image_surface_create_from_png("资料保存.png");
    Surfacesave1 = cairo_image_surface_create_from_png("资料保存2.png");
    Surfacecancel = cairo_image_surface_create_from_png("资料取消.png");
    Surfacecancel1 = cairo_image_surface_create_from_png("资料取消2.png");
    Surfaceend = cairo_image_surface_create_from_png("关闭按钮1.png");
    Surfaceend1 = cairo_image_surface_create_from_png("关闭按钮2.png");
    Surfaceend2 = cairo_image_surface_create_from_png("关闭按钮3.png");

    Infobackground = gtk_image_new_from_surface(Surfaceback);
    Infosave = gtk_image_new_from_surface(Surfacesave);
    Infocancel = gtk_image_new_from_surface(Surfacecancel);
    Infoguanbi = gtk_image_new_from_surface(Surfaceend);
    gtk_widget_set_size_request(GTK_WIDGET(Infobackground), 550, 488);

}

static void destroy_infosurfaces() {
    g_print("destroying infoface");
    cairo_surface_destroy(Surfaceback);
    cairo_surface_destroy(Surfacesave);
    cairo_surface_destroy(Surfacesave1);
    cairo_surface_destroy(Surfacecancel);
    cairo_surface_destroy(Surfacecancel1);
    cairo_surface_destroy(Surfaceend);
    cairo_surface_destroy(Surfaceend1);
    cairo_surface_destroy(Surfaceend2);
}

//背景的eventbox拖曳窗口
static gint Infobackg_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1) { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                event->x_root, event->y_root, event->time);
    }
    return 0;
}

//保存
//鼠标点击事件
static gint save_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//保存
//鼠标抬起事件
static gint save_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        //newsockfd();
    }

    return 0;
}

//保存
//鼠标移动事件
static gint save_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) Infosave, Surfacesave1); //置换图标
    return 0;
}

//保存
//鼠标离开事件
static gint save_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) Infosave, Surfacesave);
    return 0;
}

//取消
//鼠标点击事件
static gint cancel_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//取消
//鼠标抬起事件
static gint cancel_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        destroy_infosurfaces();
        gtk_widget_destroy(Infowind);
    }

    return 0;
}

//取消
//鼠标移动事件
static gint cancel_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) Infocancel, Surfacecancel1); //置换图标
    return 0;
}

//取消
//鼠标离开事件
static gint cancel_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) Infocancel, Surfacecancel);

    return 0;
}

//关闭
//鼠标点击事件
static gint guanxx_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) Infoguanbi, Surfaceend1); //置换图标
    }
    return 0;
}

//关闭
//鼠标抬起事件
static gint guanxx_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        destroy_infosurfaces();
        gtk_widget_destroy(Infowind);
    }
    return 0;
}

//关闭
//鼠标移动事件
static gint guanxx_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) Infoguanbi, Surfaceend2);
    return 0;
}

//关闭
//鼠标离开事件
static gint guanxx_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) Infoguanbi, Surfaceend);
    return 0;
}

/*//备注
//鼠标点击事件
static gint inote_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        GtkWidget *newnote;
        newnote = gtk_entry_new();
        gtk_fixed_put(GTK_FIXED(Infolayout), newnote, 48, 195);
    }
    return 0;
}
//备注
//鼠标抬起事件
static gint inote_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
    }
    return 0;
}
//备注
//鼠标移动事件
static gint inote_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));
    return 0;
}
//备注
//鼠标离开事件
static gint inote_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    return 0;
}*/

void infotv() {

    char infohead[80] = {0};
    sprintf(infohead, "%s/.momo/%u/head.png", getpwuid(getuid())->pw_dir, CurrentUserInfo.uid);
    surfacehead = cairo_image_surface_create_from_png(infohead);
    headicon = gtk_image_new_from_surface(surfacehead);
    gtk_fixed_put(GTK_FIXED(Infolayout), headicon, 23, 16);

    char idstring[80] = {0};
    sprintf(idstring, "%d", CurrentUserInfo.uid);
    iid = gtk_label_new(idstring);
    gtk_fixed_put(GTK_FIXED(Infolayout), iid, 35, 173);

    //gtk_text_view_set_border_window_size(inote, GTK_TEXT_WINDOW_LEFT, 10);
    //gtk_entry_set_has_frame((GtkEntry *) inote, FALSE);
    inote = gtk_entry_new();
    gtk_widget_set_size_request(inote, 3, 3);
    gtk_entry_set_text(inote, CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), inote, 48, 188);
    //gtk_entry_set_inner_border(<#(GtkEntry*)entry#>, <#(const GtkBorder*)border#>)
    //GtkStyleContext *context = gtk_widget_get_style_context(inote);
    //gtk_style_context_list_classes(context);
    //gtk_style_context_add_class(context, myclass);

    iname = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), iname, 48, 244);

    isex = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), isex, 222, 244);

    ibloodtype = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), ibloodtype, 405, 244);

    ibirthday = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), ibirthday, 48, 266);

    iconstellation = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), iconstellation, 222, 266);

    icity = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), icity, 222, 287);

    iprovinces = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), iprovinces, 48, 287);

    iphonenumber = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), iphonenumber, 48, 340);

    ischoolrecord = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), ischoolrecord, 48, 362);

    ihometown = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), ihometown, 48, 408);

    ihome = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), ihome, 63, 430);

    izodiac = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), izodiac, 405, 266);

    icounty = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), icounty, 405, 287);

    itel = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), itel, 295, 340);

    iprofessional = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), iprofessional, 295, 362);

    ischool = gtk_label_new(CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), ischool, 75, 385);

}

int info() {

    Infowind = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(Infowind), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (Infowind), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(Infowind), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(Infowind), 550, 488);

    /*Infobackg_event_box = gtk_event_box_new();
    Save_event_box = gtk_event_box_new();
    Cancel_event_box = gtk_event_box_new();
    Guanxx_event_box = gtk_event_box_new();*/

    Infolayout = gtk_fixed_new();
    create_infofaces();
    gtk_container_add(GTK_CONTAINER(Infowind), Infolayout);

    Infobackg_event_box = BuildEventBox(
            Infobackground,
            G_CALLBACK(Infobackg_button_press_event),
            NULL,
            NULL,
            NULL,
            NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Infobackg_event_box, 0, 0);

    Save_event_box = BuildEventBox(
            Infosave,
            G_CALLBACK(save_button_press_event),
            G_CALLBACK(save_enter_notify_event),
            G_CALLBACK(save_leave_notify_event),
            G_CALLBACK(save_button_release_event),
            NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Save_event_box, 350, 440);

    Cancel_event_box = BuildEventBox(
            Infocancel,
            G_CALLBACK(cancel_button_press_event),
            G_CALLBACK(cancel_enter_notify_event),
            G_CALLBACK(cancel_leave_notify_event),
            G_CALLBACK(cancel_button_release_event),
            NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Cancel_event_box, 450, 440);

    Guanxx_event_box = BuildEventBox(
            Infoguanbi,
            G_CALLBACK(guanxx_button_press_event),
            G_CALLBACK(guanxx_enter_notify_event),
            G_CALLBACK(guanxx_leave_notify_event),
            G_CALLBACK(guanxx_button_release_event),
            NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Guanxx_event_box, 509, 0);

    infotv();

    /*Inote_event_box=BuildEventBox(
            inote,
            G_CALLBACK(inote_button_press_event),
            G_CALLBACK(inote_enter_notify_event),
            G_CALLBACK(inote_leave_notify_event),
            G_CALLBACK(inote_button_release_event),
            NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Inote_event_box, 48, 195);*/

    gtk_widget_show_all(Infowind);
}