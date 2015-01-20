#include <gtk/gtk.h>
#include <string.h>
#include <logger.h>
#include <stdlib.h>
#include <protocol/status/Failure.h>
#include "common.h"
#include "PopupWinds.h"
#include "ClientSockfd.h"
#include "MainInterface.h"

//设置界面的控件
static GtkWidget *SetupWind, *SetupLayout;
static GtkWidget *SetupBackground, *SetupSave, *SetupCancel, *SetupGuanbi;
static cairo_surface_t *SurSetBack, *SurSetSave, *SurSetSave1, *SurSetCancel, *SurSetCancel1, *SurSetEnd, *SurSetEnd1, *SurSetEnd2;
static GtkWidget *OldPasswd, *NewPasswd1, *NewPasswd2;

static void create_setfaces()
{

    SurSetBack = ChangeThem_png("设置.png");
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

    gtk_widget_set_size_request(GTK_WIDGET(SetupBackground), 570, 500);
}

static void destroy_setsurfaces()
{
    g_print("destroying setface");
    cairo_surface_destroy(SurSetBack);
    cairo_surface_destroy(SurSetSave);
    cairo_surface_destroy(SurSetSave1);
    cairo_surface_destroy(SurSetCancel);
    cairo_surface_destroy(SurSetCancel1);
    cairo_surface_destroy(SurSetEnd);
    cairo_surface_destroy(SurSetEnd1);
    cairo_surface_destroy(SurSetEnd2);
}

int chenggong(void *data)
{
    popup("莫默告诉你：", "恭喜你修改成功");//调用提示框函数提醒用户
    /*修改成功关闭修改界面*/
    destroy_setsurfaces();
    gtk_widget_destroy(SetupWind);
    MarkNewpasswd = 0;//并将设置按钮标志置为0即可打开状态
    return 0;
}

int shibai(void *reason)
{
    /*修改密码失败的原因*/
    popup("莫默告诉你：", reason);//调用提示框函数提醒用户
    free(reason);//释放malloc分配的内存
    return 0;
}

/*处理服务端的回复信息函数*/
int dealwith_passwd(CRPBaseHeader *header, void *data)
{
    switch (header->packetID)
    {
        case CRP_PACKET_OK:
        {
            g_idle_add(chenggong, NULL);//接收到成功信息处理函数
            return 0;
        };
        case CRP_PACKET_FAILURE:
        {
            CRPPacketFailure *infodata = CRPFailureCast(header);
            char *failreason = (char *) malloc(strlen(infodata->reason));
            memcpy(failreason, infodata->reason, strlen(infodata->reason - 1));
            g_idle_add(shibai, failreason);//接收到失败信息的处理函数
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
    oldstrs = gtk_entry_get_text(GTK_ENTRY(OldPasswd));//获取输入内容
    newstrs1 = gtk_entry_get_text(GTK_ENTRY(NewPasswd1));//获取输入内容
    newstrs2 = gtk_entry_get_text(GTK_ENTRY(NewPasswd2));//获取输入内容

    /*当三个文本输入框都不为空时*/
    if ((strlen(oldstrs) != 0) && (strlen(newstrs1) != 0) && (strlen(newstrs2) != 0))
    {
        /*判断两次密码输入是否相同*/
        if (strcmp(newstrs1, newstrs2) != 0)
        {
            popup("莫默告诉你：", "两次密码不一致");//调用提示框函数提醒用户
            return 1;
        }
        else
        {
            unsigned char hash1[16], hash2[16];
            MD5((unsigned char *) oldstrs, strlen(oldstrs), hash1);//MD5哈希加密
            MD5((unsigned char *) newstrs2, strlen(newstrs2), hash2);//MD5哈希加密
            /*向服务端发送修改密码请求*/
            session_id_t passid = CountSessionId();//生成唯一的SessionId
            AddMessageNode(passid, dealwith_passwd, NULL);//处理服务端回复函数
            CRPInfoPasswordChangeSend(sockfd, passid, hash1, hash2);
        }
    }
    else
    {
        popup("莫默告诉你：", "请完善修改信息");//调用提示框函数提醒用户
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
        gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_HAND2));//设置鼠标光标
    }
    return 0;
}

//保存
//鼠标抬起事件
static gint save_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {
        change_password();//保存按钮调用修改密码处理函数
    }
    return 0;
}

//保存
//鼠标移动事件
static gint save_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) SetupSave, SurSetSave1); //置换图片
    return 0;
}

//保存
//鼠标离开事件
static gint save_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) SetupSave, SurSetSave);//置换图片
    return 0;
}

//取消
//鼠标点击事件
static gint cancel_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {
        gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_HAND2));//设置鼠标光标
    }
    return 0;
}

//取消
//鼠标抬起事件
static gint cancel_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {
        //取消按钮跟关闭XX效果一致
        destroy_setsurfaces();
        gtk_widget_destroy(SetupWind);
        MarkNewpasswd = 0;//设置按钮可点击标志
    }
    return 0;
}

//取消
//鼠标移动事件
static gint cancel_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) SetupCancel, SurSetCancel1); //置换图片
    return 0;
}

//取消
//鼠标离开事件
static gint cancel_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) SetupCancel, SurSetCancel);//置换图片
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
        //关闭按钮摧毁该界面
        destroy_setsurfaces();
        gtk_widget_destroy(SetupWind);
        MarkNewpasswd = 0;//设置按钮可点击标志
    }
    return 0;
}

//关闭
//鼠标移动事件
static gint guanxx_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) SetupGuanbi, SurSetEnd2);//置换图片
    return 0;
}

//关闭
//鼠标离开事件
static gint guanxx_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(SetupWind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) SetupGuanbi, SurSetEnd);//置换图片
    return 0;
}


int SetupFace()
{
    /*设置界面的按钮事件*/
    static GtkEventBox *Setbackg_event_box, *Save_event_box, *Cancel_event_box, *Guanxx_event_box;
    SetupWind = gtk_window_new(GTK_WINDOW_TOPLEVEL);//创建新窗口
    gtk_window_set_position(GTK_WINDOW(SetupWind), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW(SetupWind), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(SetupWind), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(SetupWind), 570, 500);//设置窗口大小

    SetupLayout = gtk_fixed_new();//创建布局容纳控件
    create_setfaces();
    gtk_container_add(GTK_CONTAINER(SetupWind), SetupLayout);//将layout加入到窗体中

    Setbackg_event_box = BuildEventBox(SetupBackground,
                                       G_CALLBACK(setbackg_button_press_event),
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL);
    gtk_fixed_put(GTK_FIXED(SetupLayout), (GtkWidget *) Setbackg_event_box, 0, 0);

    Save_event_box = BuildEventBox(SetupSave,
                                   G_CALLBACK(save_button_press_event),
                                   G_CALLBACK(save_enter_notify_event),
                                   G_CALLBACK(save_leave_notify_event),
                                   G_CALLBACK(save_button_release_event),
                                   NULL,
                                   NULL);
    gtk_fixed_put(GTK_FIXED(SetupLayout), (GtkWidget *) Save_event_box, 350, 440);

    Cancel_event_box = BuildEventBox(SetupCancel,
                                     G_CALLBACK(cancel_button_press_event),
                                     G_CALLBACK(cancel_enter_notify_event),
                                     G_CALLBACK(cancel_leave_notify_event),
                                     G_CALLBACK(cancel_button_release_event),
                                     NULL,
                                     NULL);
    gtk_fixed_put(GTK_FIXED(SetupLayout), (GtkWidget *) Cancel_event_box, 450, 440);

    Guanxx_event_box = BuildEventBox(SetupGuanbi,
                                     G_CALLBACK(guanxx_button_press_event),
                                     G_CALLBACK(guanxx_enter_notify_event),
                                     G_CALLBACK(guanxx_leave_notify_event),
                                     G_CALLBACK(guanxx_button_release_event),
                                     NULL,
                                     NULL);
    gtk_fixed_put(GTK_FIXED(SetupLayout), (GtkWidget *) Guanxx_event_box, 531, 0);

    OldPasswd = gtk_entry_new();//原密码
    NewPasswd1 = gtk_entry_new();//新密码1
    NewPasswd2 = gtk_entry_new();//新密码2
    gtk_entry_set_max_length(GTK_ENTRY(OldPasswd), 20);//设置可输入的最大长度
    gtk_entry_set_max_length(GTK_ENTRY(NewPasswd1), 20);//设置可输入的最大长度
    gtk_entry_set_max_length(GTK_ENTRY(NewPasswd2), 20);//设置可输入的最大长度
    gtk_entry_set_visibility(GTK_ENTRY(OldPasswd), FALSE);//设置密码不可见
    gtk_entry_set_invisible_char(GTK_ENTRY(OldPasswd), '*');//不可见成*
    gtk_entry_set_visibility(GTK_ENTRY(NewPasswd1), FALSE);//设置密码不可见
    gtk_entry_set_invisible_char(GTK_ENTRY(NewPasswd1), '*');//不可见成*
    gtk_entry_set_visibility(GTK_ENTRY(NewPasswd2), FALSE);//设置密码不可见
    gtk_entry_set_invisible_char(GTK_ENTRY(NewPasswd2), '*');//不可见成*
    gtk_fixed_put(GTK_FIXED(SetupLayout), OldPasswd, 100, 192);//将密码输入框放到窗体中
    gtk_fixed_put(GTK_FIXED(SetupLayout), NewPasswd1, 100, 262);//将密码输入框放到窗体中
    gtk_fixed_put(GTK_FIXED(SetupLayout), NewPasswd2, 100, 332);//将密码输入框放到窗体中

    gtk_widget_show_all(SetupWind);//显示所有在SetupWind中的控件
    return 0;
}