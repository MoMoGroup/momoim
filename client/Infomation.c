#include <gtk/gtk.h>
#include <imcommon/friends.h>
#include <pwd.h>
#include <string.h>
#include <logger.h>
#include <stdlib.h>
#include "ClientSockfd.h"
#include "common.h"
#include "MainInterface.h"

static GtkWidget *Infowind;
static GtkWidget *Infolayout;
static GtkEventBox *Infobackg_event_box, *Save_event_box, *Cancel_event_box, *Guanxx_event_box;
static GtkWidget *Infobackground, *Infosave, *Infocancel, *Infoguanbi;
static cairo_surface_t *Surfaceback, *Surfacesave, *Surfacesave1, *Surfacecancel, *Surfacecancel1, *Surfaceend, *Surfaceend1, *Surfaceend2;

static GtkWidget *iid, *isex, *inickname, *iname, *ibirthday, *iconstellation, *iprovinces, *icity;
static GtkWidget *itel, *ischool, *ipostcode, *ihometown;
static cairo_surface_t *surfacehead;
static GtkWidget *headicon;
static GtkEventBox *Inote_event_box;

static void create_infofaces() {

    Surfaceback = cairo_image_surface_create_from_png("资料.png");
    Surfacesave = cairo_image_surface_create_from_png("更新.png");
    Surfacesave1 = cairo_image_surface_create_from_png("更新2.png");
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

int infoupdate(CRPBaseHeader *header, void *data)//资料更新处理函数
{
    switch (header->packetID) {
        case CRP_PACKET_OK: {
            log_info("请求添加", "收到OKBAO\n");
            return 0;
        };
        case CRP_PACKET_FAILURE: {
            CRPPacketFailure *infodata = CRPFailureCast(header);
            log_info("FAILURe reason", infodata->reason);
            break;
        };
    }
    return 0;
}

int infosockfd() {
    UserInfo weinfo = CurrentUserInfo;
    const gchar *buf;
    buf = gtk_entry_get_text(GTK_ENTRY(inickname));
    memcpy(weinfo.nickName, buf, strlen(buf));
    log_info("更新资料", weinfo.nickName);

    buf = gtk_entry_get_text(GTK_ENTRY(iname));
    memcpy(weinfo.name, buf, strlen(buf));
    log_info("更新资料", weinfo.name);

    // buf = gtk_entry_get_text(GTK_ENTRY(isex));
    buf = gtk_combo_box_text_get_active_text((GtkComboBoxText *) isex);
    //weinfo.sex = *buf;
    log_info("更新资料", "%c", weinfo.sex);

    buf = gtk_entry_get_text(GTK_ENTRY(ibirthday));
    memcpy(weinfo.birthday, buf, strlen(buf));
    log_info("更新资料", weinfo.birthday);

    //buf = gtk_entry_get_text(GTK_ENTRY(iconstellation));
    buf = gtk_combo_box_text_get_active_text((GtkComboBoxText *) iconstellation);
    //weinfo.constellation = *buf;
    log_info("更新资料", "%c", weinfo.constellation);

    buf = gtk_entry_get_text(GTK_ENTRY(iprovinces));
    memcpy(weinfo.provinces, buf, strlen(buf));
    log_info("更新资料", weinfo.provinces);

    buf = gtk_entry_get_text(GTK_ENTRY(icity));
    memcpy(weinfo.city, buf, strlen(buf));
    log_info("更新资料", weinfo.city);

    buf = gtk_entry_get_text(GTK_ENTRY(itel));
    memcpy(weinfo.tel, buf, strlen(buf));
    log_info("更新资料", weinfo.tel);

    buf = gtk_entry_get_text(GTK_ENTRY(ipostcode));
    /*if(strcmp("dd", <#(const char*)__s2#>))
    {

    }*/
    memcpy(weinfo.postcode, buf, strlen(buf));
    log_info("更新资料", weinfo.postcode);

    buf = gtk_entry_get_text(GTK_ENTRY(ischool));
    memcpy(weinfo.school, buf, strlen(buf));
    log_info("更新资料", weinfo.school);

    buf = gtk_entry_get_text(GTK_ENTRY(ihometown));
    memcpy(weinfo.hometown, buf, strlen(buf));
    log_info("更新资料", weinfo.hometown);

    session_id_t newinfoid = CountSessionId();
    AddMessageNode(newinfoid, infoupdate, NULL);
    CRPInfoDataSend(sockfd, newinfoid, 0, &weinfo);
    return 0;
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
        infosockfd();
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

void infotv() {

    char infohead[80] = {0};
    sprintf(infohead, "%s/.momo/%u/head.png", getpwuid(getuid())->pw_dir, CurrentUserInfo.uid);
    surfacehead = cairo_image_surface_create_from_png(infohead);
    headicon = gtk_image_new_from_surface(surfacehead);
    gtk_fixed_put(GTK_FIXED(Infolayout), headicon, 23, 16);

    char idstring[80] = {0};//id
    sprintf(idstring, "%d", CurrentUserInfo.uid);
    iid = gtk_label_new(idstring);
    gtk_fixed_put(GTK_FIXED(Infolayout), iid, 240, 30);

    inickname = gtk_entry_new();//昵称
    gtk_entry_set_max_length(inickname, 8);
    gtk_entry_set_has_frame((GtkEntry *) inickname, FALSE);
    gtk_entry_set_text(inickname, CurrentUserInfo.nickName);
    gtk_fixed_put(GTK_FIXED(Infolayout), inickname, 58, 175);

    iname = gtk_entry_new();//姓名
    gtk_entry_set_max_length(iname, 4);
    gtk_entry_set_has_frame((GtkEntry *) iname, FALSE);
    gtk_entry_set_text(iname, CurrentUserInfo.name);
    gtk_fixed_put(GTK_FIXED(Infolayout), iname, 48, 235);

    isex = gtk_combo_box_text_new();
    gtk_combo_box_text_append(isex, "0", "女");
    gtk_combo_box_text_append(isex, "1", "男");
    //gtk_entry_set_has_frame((GtkEntry *) isex, FALSE);
    gtk_fixed_put(GTK_FIXED(Infolayout), isex, 305, 225);

    ibirthday = gtk_entry_new();
    gtk_entry_set_max_length(ibirthday, 10);
    gtk_entry_set_has_frame((GtkEntry *) ibirthday, FALSE);
    gtk_entry_set_text(ibirthday, CurrentUserInfo.birthday);
    gtk_fixed_put(GTK_FIXED(Infolayout), ibirthday, 48, 263);

    iconstellation = gtk_combo_box_text_new();
    gtk_entry_set_has_frame((GtkEntry *) iconstellation, FALSE);
    gtk_combo_box_text_append(iconstellation, "1", "水瓶座");
    gtk_combo_box_text_append(iconstellation, "2", "双鱼座");
    gtk_combo_box_text_append(iconstellation, "3", "白羊座");
    gtk_combo_box_text_append(iconstellation, "4", "金牛座");
    gtk_combo_box_text_append(iconstellation, "5", "双子座");
    gtk_combo_box_text_append(iconstellation, "6", "巨蟹座");
    gtk_combo_box_text_append(iconstellation, "7", "狮子座");
    gtk_combo_box_text_append(iconstellation, "8", "处女座");
    gtk_combo_box_text_append(iconstellation, "9", "天秤座");
    gtk_combo_box_text_append(iconstellation, "10", "天蝎座");
    gtk_combo_box_text_append(iconstellation, "11", "射手座");
    gtk_combo_box_text_append(iconstellation, "12", "摩羯座");
    /*GtkScrolledWindow *swxz;
    swxz = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_container_add(GTK_CONTAINER(swxz),iconstellation);*/
    gtk_entry_set_text(iconstellation, CurrentUserInfo.constellation);
    gtk_fixed_put(GTK_FIXED(Infolayout), iconstellation, 305, 260);
    //gtk_widget_set_size_request(GTK_WIDGET(swxz), 10, 40);

    iprovinces = gtk_entry_new();
    gtk_entry_set_max_length(iprovinces, 3);
    gtk_entry_set_has_frame((GtkEntry *) iprovinces, FALSE);
    gtk_entry_set_text(iprovinces, CurrentUserInfo.provinces);
    gtk_fixed_put(GTK_FIXED(Infolayout), iprovinces, 48, 290);

    icity = gtk_entry_new();
    gtk_entry_set_max_length(icity, 6);
    gtk_entry_set_has_frame((GtkEntry *) icity, FALSE);
    gtk_entry_set_text(icity, CurrentUserInfo.city);
    gtk_fixed_put(GTK_FIXED(Infolayout), icity, 305, 290);

    itel = gtk_entry_new();
    gtk_entry_set_max_length(itel, 11);
    gtk_entry_set_has_frame((GtkEntry *) itel, FALSE);
    gtk_entry_set_text(itel, CurrentUserInfo.tel);
    gtk_fixed_put(GTK_FIXED(Infolayout), itel, 48, 347);

    ipostcode = gtk_entry_new();
    gtk_entry_set_max_length(ipostcode, 6);
    gtk_entry_set_has_frame((GtkEntry *) ipostcode, FALSE);
    gtk_entry_set_text(ipostcode, CurrentUserInfo.postcode);
    gtk_fixed_put(GTK_FIXED(Infolayout), ipostcode, 305, 345);

    ischool = gtk_entry_new();
    gtk_entry_set_max_length(ischool, 12);
    gtk_entry_set_has_frame((GtkEntry *) ischool, FALSE);
    gtk_entry_set_text(ischool, CurrentUserInfo.school);
    gtk_fixed_put(GTK_FIXED(Infolayout), ischool, 75, 375);

    ihometown = gtk_entry_new();
    gtk_entry_set_max_length(ihometown, 30);
    gtk_entry_set_has_frame((GtkEntry *) ihometown, FALSE);
    gtk_entry_set_text(ihometown, CurrentUserInfo.hometown);
    gtk_fixed_put(GTK_FIXED(Infolayout), ihometown, 48, 403);
}

int info() {

    Infowind = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(Infowind), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (Infowind), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(Infowind), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(Infowind), 550, 488);

    Infolayout = gtk_fixed_new();
    create_infofaces();
    gtk_container_add(GTK_CONTAINER(Infowind), Infolayout);

    Infobackg_event_box = BuildEventBox(Infobackground,
                                        G_CALLBACK(Infobackg_button_press_event),
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Infobackg_event_box, 0, 0);

    Save_event_box = BuildEventBox(Infosave,
                                   G_CALLBACK(save_button_press_event),
                                   G_CALLBACK(save_enter_notify_event),
                                   G_CALLBACK(save_leave_notify_event),
                                   G_CALLBACK(save_button_release_event),
                                   NULL,
                                   NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Save_event_box, 350, 440);

    Cancel_event_box = BuildEventBox(Infocancel,
                                     G_CALLBACK(cancel_button_press_event),
                                     G_CALLBACK(cancel_enter_notify_event),
                                     G_CALLBACK(cancel_leave_notify_event),
                                     G_CALLBACK(cancel_button_release_event),
                                     NULL,
                                     NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Cancel_event_box, 450, 440);

    Guanxx_event_box = BuildEventBox(Infoguanbi,
                                     G_CALLBACK(guanxx_button_press_event),
                                     G_CALLBACK(guanxx_enter_notify_event),
                                     G_CALLBACK(guanxx_leave_notify_event),
                                     G_CALLBACK(guanxx_button_release_event),
                                     NULL,
                                     NULL);
    gtk_fixed_put(GTK_FIXED(Infolayout), Guanxx_event_box, 509, 0);

    infotv();
    gtk_widget_show_all(Infowind);
}