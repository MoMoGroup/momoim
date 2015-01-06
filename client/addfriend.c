#include <logger.h>
#include <ftlist.h>
#include <protocol/info/Data.h>
#include <protocol/info/Request.h>
#include <protocol/CRPPackets.h>
#include <protocol/base.h>
#include <protocol/status/Failure.h>
#include "addfriend.h"
#include "common.h"
#include "ClientSockfd.h"
#include "MainInterface.h"

GtkWidget *addlayout, *addwindow,*addnextlayout;
GtkWidget *addtext;




//下一步按下效果
//static gint next_button_press_event(GtkWidget *widget,
//
//        GdkEventButton *event, gpointer data)
//{
//
//    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
//    {
//        gdk_window_set_cursor(gtk_widget_get_window(addwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
//        gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut2);
//    }
//
//    return 0;
//
//}




//关闭
static gint close_button_release_event(GtkWidget *widget, GdkEventButton *event, //下一步事件

        gpointer data)
{
    gtk_widget_destroy(addwindow);
    return 0;
}

int searchfriend(CRPBaseHeader *header, void *data)//接收查找好友的资料
{
    cairo_surface_t *surfacebackground0;

    log_info("in search friend", "\n");
    if (header->packetID == CRP_PACKET_FAILURE)
    {
        CRPPacketFailure *infodata = CRPFailureCast(header);
        log_info("FAILURe reason", infodata->reason);
        return 0;

    }
    CRPPacketInfoData *infodata = CRPInfoDataCast(header);
    log_info("USERDATA", "Nick:%s\n", infodata->info.nickName);//用户昵称是否获取成功
    gtk_widget_hide(addlayout);//隐藏原来的

    addnextlayout= gtk_fixed_new();
   // gtk_container_add((), <#(GtkWidget*)widget#>)
    surfacebackground0= cairo_image_surface_create_from_png("0.png");


    gtk_widget_show_all(addwindow);


    if ((const char *) infodata != header->data)
    {
        free(infodata);
    }
}


static gint next_button_release_event(GtkWidget *widget, GdkEventButton *event, //下一步事件

        gpointer data)
{
    //log_info("", "%s", gtk_entry_get_text(addtext));
    session_id_t sessionid = CountSessionId();

    AddMessageNode(sessionid, searchfriend, "da");//主消息循环注册查找好友
    CRPInfoRequestSend(sockfd, sessionid, atol(gtk_entry_get_text(addtext)) ); //请求要添加的资料
    log_info("注册之后", "请求之后\n");


    return 0;
}

int AddFriendFun()
{
    cairo_surface_t *surfacebackground, *surfacebiaoji, *surfacenext, *surfacenext_press, *surfaceclose;//资源
    GtkWidget *background, *biaoji, *next, *next_press, *close;    //引用

    GtkEventBox *next_enent_box, *close_event_box;//事件盒子


    gtk_window_set_position(GTK_WINDOW(addwindow), GTK_WIN_POS_CENTER);//窗口出现位置
    gtk_window_set_resizable(GTK_WINDOW (addwindow), FALSE);//窗口不可改变
    gtk_window_set_decorated(GTK_WINDOW(addwindow), FALSE);   // 去掉边框

    addwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    addlayout = gtk_fixed_new();

//加载资源
    surfacebackground = cairo_image_surface_create_from_png("查找背景.png");
    surfacebiaoji = cairo_image_surface_create_from_png("标记.png");
    surfacenext = cairo_image_surface_create_from_png("下一步.png");
    surfacenext_press = cairo_image_surface_create_from_png("下一步2.png");
    surfaceclose = cairo_image_surface_create_from_png("关闭按钮1.png");

    background = gtk_image_new_from_surface(surfacebackground);
    biaoji = gtk_image_new_from_surface(surfacebiaoji);
    next = gtk_image_new_from_surface(surfacenext);
    close = gtk_image_new_from_surface(surfaceclose);

    next_enent_box = BuildEventBox(
            next,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(next_button_release_event),
            NULL
    );
    close_event_box = BuildEventBox(
            close,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(close_button_release_event),
            NULL
    );


    gtk_fixed_put(GTK_FIXED(addlayout), background, 0, 0);
    gtk_fixed_put(GTK_FIXED(addlayout), biaoji, 6, 75);
    gtk_fixed_put(GTK_FIXED(addlayout), next_enent_box, 400, 200);
    gtk_fixed_put(GTK_FIXED(addlayout), close_event_box, 519, 0);


    addtext = gtk_entry_new();//帐号输入
    gtk_fixed_put(GTK_FIXED(addlayout), addtext, 170, 80);


    gtk_container_add(GTK_CONTAINER (addwindow), addlayout);
    gtk_widget_show_all(addwindow);
}


