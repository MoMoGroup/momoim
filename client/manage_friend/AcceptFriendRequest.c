#include <common.h>
#include <stdlib.h>
#include <string.h>
#include "friend.h"

//以下函数为添加好友提示框，同意或者或略。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
typedef struct tongyi
{
    uint32_t uid;
    GtkWidget *win;
    char *verification_message;
    char nickname[32];
} tongyi;

//取消的话直接销毁
gint popup_cancel(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GtkWidget *win = GTK_WIDGET(data);

    gtk_widget_destroy(win);
    return 0;
}

gint popup_done(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    tongyi *info = data;
    CRPFriendAcceptSend(sockfd, 1, info->uid);//同意的话发送Accept
    gtk_widget_destroy(info->win);
    return 0;
}

//背景的eventbox拖曳窗口
static gint pop_mov(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GtkWidget *win = GTK_WIDGET(data);

    gdk_window_set_cursor(gtk_widget_get_window(win), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   (gint) event->x_root, (gint) event->y_root, event->time);
    }
    return 0;
}

int put(void *data)
{
    tongyi *info = data;
    GtkEventBox *popup_accept_eventbox, *popup_cancel_eventbox, *pop_mov_event;
    GtkWidget *popupwindow, *popupframelayout, *popuplayout;
    cairo_surface_t *popupsurfacecancel, *popupsurfacedone;
    cairo_surface_t *popupsurfacebackground;

    GtkWidget *popupcancel, *popupdone, *popupbackground;


//        popupwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    popupwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    popupframelayout = gtk_layout_new(NULL, NULL);
    popuplayout = gtk_fixed_new();

    gtk_window_set_position(GTK_WINDOW(popupwindow), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (popupwindow), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(popupwindow), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(popupwindow), 250, 235);


    popupsurfacecancel = ChangeThem_png("忽略1.png");
    popupsurfacedone = ChangeThem_png("同意1.png");
    popupsurfacebackground = ChangeThem_png("提示框.png");
    //获得
    popupcancel = gtk_image_new_from_surface(popupsurfacecancel);
    popupdone = gtk_image_new_from_surface(popupsurfacedone);


    popupbackground = gtk_image_new_from_surface(popupsurfacebackground);

//    tongyi *info = malloc(sizeof(struct tongyi));
//    info->uid = uid;
    info->win = popupwindow;

    // 设置窗体获取鼠标事件
    pop_mov_event = BuildEventBox(
            popupbackground,
            G_CALLBACK(pop_mov),
            NULL,
            NULL,
            NULL,
            NULL,
            popupwindow);

    gtk_fixed_put(GTK_FIXED(popuplayout), GTK_WIDGET(pop_mov_event), 0, 0);


    popup_cancel_eventbox = BuildEventBox(
            popupcancel,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(popup_cancel),
            NULL,
            popupwindow);
    popup_accept_eventbox = BuildEventBox(
            popupdone,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(popup_done),
            NULL,
            info);


    char buf[80], mes[256];

    GtkWidget *yanzheng, *text, *xitong;

    sprintf(buf, "用户%d,昵称:%s\n请求添加你为好友。", info->uid, info->nickname);
    sprintf(mes, "验证消息:%s", info->verification_message);


    xitong = gtk_label_new("系统消息");
    text = gtk_label_new(buf);

    yanzheng = gtk_label_new(mes);


    gtk_fixed_put(GTK_FIXED(popuplayout), xitong, 14, 10);//标题
    gtk_fixed_put(GTK_FIXED(popuplayout), GTK_WIDGET(text), 30, 50);//添加信息
    gtk_fixed_put(GTK_FIXED(popuplayout), GTK_WIDGET(yanzheng), 30, 100);//验证信息
    gtk_fixed_put(GTK_FIXED(popuplayout), GTK_WIDGET(popup_cancel_eventbox), 30, 170);
    gtk_fixed_put(GTK_FIXED(popuplayout), GTK_WIDGET(popup_accept_eventbox), 150, 170);


    gtk_container_add(GTK_CONTAINER (popupwindow), popupframelayout);
    gtk_container_add(GTK_CONTAINER (popupframelayout), popuplayout);

    gtk_widget_show_all(popupwindow);
    return 0;
}

//给一个uid，拿到昵称的函数
int get_nicheng(CRPBaseHeader *header, void *data)
{
    tongyi *aaaa;
    aaaa = (tongyi *) data;

    if (header->packetID == CRP_PACKET_INFO_DATA)//查询到资料，防到通知框
    {
        CRPPacketInfoData *infodata = CRPInfoDataCast(header);
        memcpy(aaaa->nickname, infodata->info.nickName, sizeof(infodata->info.nickName));
        g_idle_add(put, aaaa);

        if ((void *) infodata == header)
        {
            free(infodata);
        }
        return 0;
    }


}

int Friend_Request_Popup(uint32_t uid, const char *verification_message)
{
    tongyi *info = malloc(sizeof(struct tongyi));
    info->uid = uid;
    info->verification_message = verification_message;
    session_id_t sessionid = CountSessionId();
    AddMessageNode(sessionid, get_nicheng, info);
    CRPInfoRequestSend(sockfd, sessionid, uid);//请求添加方资料
    return 0;

}
