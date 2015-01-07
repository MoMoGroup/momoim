#include <logger.h>
#include <ftlist.h>
#include <protocol/info/Data.h>
#include <protocol/info/Request.h>
#include <protocol/CRPPackets.h>
#include <pwd.h>
#include <glib-unix.h>
#include <math.h>
#include <imcommon/friends.h>
#include <cairo-script-interpreter.h>
#include "addfriend.h"
#include "common.h"
#include "ClientSockfd.h"
#include "MainInterface.h"

GtkWidget *addwindow, *addframelayout;
GtkWidget *addlayout2, *addlayout1;//layout

GtkWidget *addtext;

cairo_surface_t *surfacebackground, *surfacebiaoji, *surfacenext, *surfacenext_press, *surfaceclose;
cairo_surface_t *surfacebackground0;
cairo_surface_t *surfacehead;
//资源

GtkWidget *background, *biaoji, *next, *next_press, *addclose;    //引用
GtkWidget *head, *smallhead;

GtkEventBox *next_enent_box, *close_event_box;
//事件盒子

GtkWidget *nicheng;

GtkWidget *background0;

typedef struct ao {
    FILE *fp;
    char *nickname;
    char key[16];
};
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


gboolean first(gpointer user_data)
{
    addlayout2 = gtk_fixed_new();
    surfacebackground0 = cairo_image_surface_create_from_png("查找2.png");
    background0 = gtk_image_new_from_surface(surfacebackground0);

    gtk_fixed_put(GTK_FIXED(addlayout2), background0, 0, 0);
    return FALSE;
}

gboolean putnickname(gpointer user_data)
{


    log_info("putnickname", "昵称%s\n", user_data);
    // gtk_widget_show_all(addlayout2);//显示2

//    gtk_fixed_put(GTK_FIXED(addlayout2), close_event_box, 519, 0);

    return FALSE;
}

gboolean putuid(gpointer user_data)
{

    log_info("uid", "%u\n", user_data);

    return FALSE;
}

gboolean putimage(gpointer user_data)
{
    struct ao *p = user_data;
    static cairo_t *cr;
    cairo_surface_t *surface;

    //把昵称显示出来
    nicheng = gtk_label_new(p->nickname);
    free(p->nickname);
    PangoFontDescription *font;
    font = pango_font_description_from_string("Sans");//"Sans"字体名
    pango_font_description_set_size(font, 20 * PANGO_SCALE);//设置字体大小
    gtk_widget_override_font(nicheng, font);
    gtk_fixed_put(GTK_FIXED(addlayout2), nicheng, 60, 180);

    char filename[256];
    HexadecimalConversion(filename, p->key);
    surface = cairo_image_surface_create_from_png(filename);
    int w = cairo_image_surface_get_width(surface);
    int h = cairo_image_surface_get_height(surface);

    //创建画布
    surfacehead = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 300, 150);
    //创建画笔
    cr = cairo_create(surfacehead);
    //缩放
    cairo_arc(cr, 60, 60, 60, 0, M_PI * 2);
    cairo_clip(cr);
    cairo_scale(cr, 125.0 / w, 126.0 / h);
    //把画笔和图片相结合。
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    smallhead = gtk_image_new_from_surface(surfacehead);
    gtk_fixed_put(GTK_FIXED(addlayout2), smallhead, 27, 15);
    cairo_destroy(cr);


    gtk_widget_hide(addlayout1);//隐藏1

    gtk_container_add(GTK_CONTAINER (addframelayout), addlayout2);

    gtk_widget_show_all(addlayout2);//显示2
    return FALSE;
}


//关闭
static gint close_button_release_event(GtkWidget *widget, GdkEventButton *event, //下一步事件

        gpointer data)
{
    gtk_widget_destroy(addwindow);
    return 0;
}

int searchfriend(CRPBaseHeader *header, void *data)//接收查找好友的资料
{
    g_idle_add(first, "");
    struct ao *p = (struct ao *) data;
    switch (header->packetID)
    {

        case CRP_PACKET_FAILURE:
        {
            CRPPacketFailure *infodata = CRPFailureCast(header);
            log_info("FAILURe reason", infodata->reason);
            break;
        };
        case CRP_PACKET_INFO_DATA:
        {
            CRPPacketInfoData *infodata = CRPInfoDataCast(header);
            //log_info("putnickname", "昵称%s\n", infodata->info.nickName);
            memcpy(p->key, infodata->info.icon, 16);
            CRPFileRequestSend(sockfd, header->sessionID, 0, infodata->info.icon);//发送用户头像请求
//            uint32_t uid=infodata->info.uid;
//            g_idle_add(putuid, uid);
            //显示昵称上去
            char *mem = malloc(strlen(infodata->info.nickName) + 1);
            memcpy(mem, infodata->info.nickName, strlen(infodata->info.nickName));
            mem[strlen(infodata->info.nickName)] = 0;
            p->nickname = mem;
            //free(mem);
            if ((const char *) infodata != header->data)
            {
                free(infodata);
            }
            break;


        };
        case CRP_PACKET_FILE_DATA_START:
        {
            CRPPacketFileDataStart *packet = CRPFileDataStartCast(header);
            char filename[256];
            HexadecimalConversion(filename, p->key);
            p->fp = fopen(filename, "w");
            CRPOKSend(sockfd, header->sessionID);
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            break;
        };

        case CRP_PACKET_FILE_DATA://接受头像
        {
            CRPPacketFileData *packet = CRPFileDataCast(header);

            fwrite(packet->data, 1, packet->length, p->fp);

            CRPOKSend(sockfd, header->sessionID);
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            break;
        };
        case CRP_PACKET_FILE_DATA_END://头像接受完
        {

            CRPPacketFileDataEnd *packet = CRPFileDataEndCast(header);

            fclose(p->fp);
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            g_idle_add(putimage, p);
            return 0;
        }

    }
    return 1;
}


static gint next_button_release_event(GtkWidget *widget, GdkEventButton *event, //点击下一步开始查找好友资料，主线程

        gpointer data)
{
    //log_info("", "%s", gtk_entry_get_text(addtext));
    session_id_t sessionid = CountSessionId();

    AddMessageNode(sessionid, searchfriend, malloc(sizeof(struct ao)));//主消息循环注册查找好友
    CRPInfoRequestSend(sockfd, sessionid, atol(gtk_entry_get_text(addtext))); //请求要添加的资料
    return 0;
}

int AddFriendFun()
{
    addwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    addframelayout = gtk_layout_new(NULL, NULL);
    addlayout1 = gtk_fixed_new();


    gtk_window_set_position(GTK_WINDOW(addwindow), GTK_WIN_POS_CENTER);//窗口出现位置
    gtk_window_set_resizable(GTK_WINDOW (addwindow), FALSE);//窗口不可改变
    gtk_window_set_decorated(GTK_WINDOW(addwindow), FALSE);   // 去掉边框

    gtk_widget_set_size_request(GTK_WIDGET(addwindow), 560, 270);


    create_surface();

    next_enent_box = BuildEventBox(
            next,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(next_button_release_event),
            NULL
    );
    close_event_box = BuildEventBox(
            addclose,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(close_button_release_event),
            NULL
    );
    gtk_fixed_put(GTK_FIXED(addlayout1), background, 0, 0);
    gtk_fixed_put(GTK_FIXED(addlayout1), biaoji, 6, 75);
    gtk_fixed_put(GTK_FIXED(addlayout1), next_enent_box, 400, 200);
    gtk_fixed_put(GTK_FIXED(addlayout1), close_event_box, 519, 0);


    addtext = gtk_entry_new();//帐号输入
    gtk_fixed_put(GTK_FIXED(addlayout1), addtext, 170, 80);

    gtk_container_add(GTK_CONTAINER (addwindow), addframelayout);
    gtk_container_add(GTK_CONTAINER (addframelayout), addlayout1);

    gtk_widget_show_all(addwindow);


}

void create_surface()
{
    //加载资源
    surfacebackground = cairo_image_surface_create_from_png("查找背景2.png");
    surfacebiaoji = cairo_image_surface_create_from_png("标记.png");
    surfacenext = cairo_image_surface_create_from_png("下一步.png");
    surfacenext_press = cairo_image_surface_create_from_png("下一步2.png");
    surfaceclose = cairo_image_surface_create_from_png("关闭按钮1.png");
//获得
    background = gtk_image_new_from_surface(surfacebackground);
    biaoji = gtk_image_new_from_surface(surfacebiaoji);
    next = gtk_image_new_from_surface(surfacenext);
    addclose = gtk_image_new_from_surface(surfaceclose);
}