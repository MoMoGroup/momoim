#include <gtk/gtk.h>
#include <string.h>
#include <logger.h>
#include <stdlib.h>
#include <protocol/status/Failure.h>
#include "common.h"
#include "PopupWinds.h"
#include "ClientSockfd.h"
#include "MainInterface.h"

static GtkWidget *SetupWind, *SetupLayout;
static GtkWidget *SetupBackground, *SetupSave, *SetupCancel, *SetupGuanbi, *JibenSet, *SafeSet, *QuanxianSet;
static cairo_surface_t *SurSetBack, *SurSetSave, *SurSetSave1, *SurSetCancel, *SurSetCancel1, *SurSetEnd, *SurSetEnd1, *SurSetEnd2;
static cairo_surface_t *SurSetJiben, *SurSetSafe, *SurSetQuanxian;
static GtkWidget *OldPasswd, *NewPasswd1, *Newpasswd2;

//int MarkNewpasswd;
static void create_setfaces()
{

    SurSetBack = ChangeThem_png("安全设置背景.png");
    SurSetSave = ChangeThem_png("保存.png");
    SurSetSave1 = ChangeThem_png("保存2.png");
    SurSetCancel = ChangeThem_png("资料取消.png");
    SurSetCancel1 = ChangeThem_png("资料取消2.png");
    SurSetEnd = ChangeThem_png("关闭按钮1.png");
    SurSetEnd1 = ChangeThem_png("关闭按钮2.png");
    SurSetEnd2 = ChangeThem_png("关闭按钮3.png");

    SetupBackground = gtk_image_new_from_surface(SurSetBack);
    SetupSave = gtk_image_new_from_surface(SurSetSave);
    SetupCancel = gtk_image_new_from_surface(SurSetCancel);
    SetupGuanbi = gtk_image_new_from_surface(SurSetEnd);

    SurSetJiben = ChangeThem_png("基本设置.png");
    SurSetSafe = ChangeThem_png("安全设置.png");
    SurSetQuanxian = ChangeThem_png("权限设置.png");
    JibenSet = gtk_image_new_from_surface(SurSetJiben);
    SafeSet = gtk_image_new_from_surface(SurSetSafe);
    QuanxianSet = gtk_image_new_from_surface(SurSetQuanxian);

    gtk_widget_set_size_request(GTK_WIDGET(SetupBackground), 570, 500);
}

static void destroy_setsurfaces()
{
    g_print("destroying infoface");
    cairo_surface_destroy(SurSetBack);
    cairo_surface_destroy(SurSetSave);
    cairo_surface_destroy(SurSetSave1);
    cairo_surface_destroy(SurSetCancel);
    cairo_surface_destroy(SurSetCancel1);
    cairo_surface_destroy(SurSetEnd);
    cairo_surface_destroy(SurSetEnd1);
    cairo_surface_destroy(SurSetEnd2);
    cairo_surface_destroy(SurSetJiben);
    cairo_surface_destroy(SurSetSafe);
    cairo_surface_destroy(SurSetQuanxian);
}

int chenggong(void *data)
{
    popup("莫默告诉你：", "恭喜你修改成功");
    destroy_setsurfaces();
    gtk_widget_destroy(SetupWind);

    MarkNewpasswd = 0;
    return 0;
}

int shibai(void *reason)
{
    log_info("FAILURe reason2", reason);
    popup("莫默告诉你：", reason);
    free(reason);
    return 0;
}

int dealwith_passwd(CRPBaseHeader *header, void *data)
{
    switch (header->packetID)
    {
        case CRP_PACKET_OK:
        {
            g_idle_add(chenggong, NULL);
            return 0;
        };
        case CRP_PACKET_FAILURE:
        {
            CRPPacketFailure *infodata = CRPFailureCast(header);
            log_info("FAILURe reason", infodata->reason);
            char *failreason = (char *) malloc(strlen(infodata->reason));
            memcpy(failreason, infodata->reason, strlen(infodata->reason - 1));
            g_idle_add(shibai, failreason);
            log_info("FAILURe reason1", failreason);
            if ((void *) infodata != header->data)
            {
                free(infodata);
            }
            break;
        };
    }
    return 0;
}

int change_password()
{
    const gchar *oldstrs, *newstrs1, *newstrs2;
    oldstrs = gtk_entry_get_text(GTK_ENTRY(OldPasswd));
    newstrs1 = gtk_entry_get_text(GTK_ENTRY(NewPasswd1));
    newstrs2 = gtk_entry_get_text(GTK_ENTRY(Newpasswd2));

    if ((strlen(oldstrs) != 0) && (strlen(newstrs1) != 0) && (strlen(newstrs2) != 0))
    {
        if (strcmp(newstrs1, newstrs2) != 0)
        {
            popup("莫默告诉你：", "两次密码不一致");
            return 1;
        }
        else
        {
            unsigned char hash1[16], hash2[16];
            MD5((unsigned char *) oldstrs, strlen(oldstrs), hash1);
            MD5((unsigned char *) newstrs2, strlen(newstrs2), hash2);
            session_id_t passid = CountSessionId();
            AddMessageNode(passid, dealwith_passwd, NULL);
            CRPInfoPasswordChangeSend(sockfd, passid, hash1, hash2);
        }
    }
    else
    {
        popup("莫默告诉你：", "请完善修改信息");
        return 1;
    }
    return 0;
}

//背景的eventbox拖曳窗口
static gint setbackg_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   event->x_root, event->y_root, event->time);
    }
    return 0;
}

//保存
//鼠标点击事件
static gint save_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    if (event->button == 1)
    {
        gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//保存
//鼠标抬起事件
static gint save_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {
        change_password();
    }
    return 0;
}

//保存
//鼠标移动事件
static gint save_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) SetupSave, SurSetSave1); //置换图标
    return 0;
}

//保存
//鼠标离开事件
static gint save_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) SetupSave, SurSetSave);
    return 0;
}

//取消
//鼠标点击事件
static gint cancel_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {
        gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//取消
//鼠标抬起事件
static gint cancel_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {
        destroy_setsurfaces();
        gtk_widget_destroy(SetupWind);
        MarkNewpasswd = 0;
    }
    return 0;
}

//取消
//鼠标移动事件
static gint cancel_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) SetupCancel, SurSetCancel1); //置换图标
    return 0;
}

//取消
//鼠标离开事件
static gint cancel_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) SetupCancel, SurSetCancel);
    return 0;
}

//关闭
//鼠标点击事件
static gint guanxx_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {
        gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) SetupGuanbi, SurSetEnd1); //置换图标
    }
    return 0;
}

//关闭
//鼠标抬起事件
static gint guanxx_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {
        destroy_setsurfaces();
        gtk_widget_destroy(SetupWind);
        MarkNewpasswd = 0;
    }
    return 0;
}

//关闭
//鼠标移动事件
static gint guanxx_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) SetupGuanbi, SurSetEnd2);
    return 0;
}

//关闭
//鼠标离开事件
static gint guanxx_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) SetupGuanbi, SurSetEnd);
    return 0;
}


int SetupFace()
{
    static GtkEventBox *Setbackg_event_box, *Save_event_box, *Cancel_event_box, *Guanxx_event_box;
    SetupWind = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(SetupWind), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW(SetupWind), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(SetupWind), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(SetupWind), 570, 500);

    SetupLayout = gtk_fixed_new();
    create_setfaces();
    gtk_container_add(GTK_CONTAINER(SetupWind), SetupLayout);

    Setbackg_event_box = BuildEventBox(SetupBackground,
                                       G_CALLBACK(setbackg_button_press_event),
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL);
    gtk_fixed_put(GTK_FIXED(SetupLayout), Setbackg_event_box, 0, 0);

    Save_event_box = BuildEventBox(SetupSave,
                                   G_CALLBACK(save_button_press_event),
                                   G_CALLBACK(save_enter_notify_event),
                                   G_CALLBACK(save_leave_notify_event),
                                   G_CALLBACK(save_button_release_event),
                                   NULL,
                                   NULL);
    gtk_fixed_put(GTK_FIXED(SetupLayout), Save_event_box, 350, 440);

    Cancel_event_box = BuildEventBox(SetupCancel,
                                     G_CALLBACK(cancel_button_press_event),
                                     G_CALLBACK(cancel_enter_notify_event),
                                     G_CALLBACK(cancel_leave_notify_event),
                                     G_CALLBACK(cancel_button_release_event),
                                     NULL,
                                     NULL);
    gtk_fixed_put(GTK_FIXED(SetupLayout), Cancel_event_box, 450, 440);

    Guanxx_event_box = BuildEventBox(SetupGuanbi,
                                     G_CALLBACK(guanxx_button_press_event),
                                     G_CALLBACK(guanxx_enter_notify_event),
                                     G_CALLBACK(guanxx_leave_notify_event),
                                     G_CALLBACK(guanxx_button_release_event),
                                     NULL,
                                     NULL);
    gtk_fixed_put(GTK_FIXED(SetupLayout), Guanxx_event_box, 531, 0);

    gtk_fixed_put(GTK_FIXED(SetupLayout), JibenSet, 10, 70);
    gtk_fixed_put(GTK_FIXED(SetupLayout), SafeSet, 200, 70);
    gtk_fixed_put(GTK_FIXED(SetupLayout), QuanxianSet, 380, 65);

    OldPasswd = gtk_entry_new();//原密码
    NewPasswd1 = gtk_entry_new();//新密码1
    Newpasswd2 = gtk_entry_new();//新密码2
    gtk_entry_set_max_length(GTK_ENTRY(OldPasswd), 20);//设置输入的最大长度
    gtk_entry_set_max_length(GTK_ENTRY(NewPasswd1), 20);
    gtk_entry_set_max_length(GTK_ENTRY(Newpasswd2), 20);
    gtk_entry_set_visibility(GTK_ENTRY(OldPasswd), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(OldPasswd), '*');
    gtk_entry_set_visibility(GTK_ENTRY(NewPasswd1), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(NewPasswd1), '*');
    gtk_entry_set_visibility(GTK_ENTRY(Newpasswd2), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(Newpasswd2), '*');
    gtk_fixed_put(GTK_FIXED(SetupLayout), OldPasswd, 240, 200);
    gtk_fixed_put(GTK_FIXED(SetupLayout), NewPasswd1, 240, 240);
    gtk_fixed_put(GTK_FIXED(SetupLayout), Newpasswd2, 240, 282);

    gtk_widget_show_all(SetupWind);
    return 0;
}