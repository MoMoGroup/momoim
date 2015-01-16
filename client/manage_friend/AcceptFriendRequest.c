#include <common.h>
#include <stdlib.h>
#include "friend.h"

//以下函数为添加好友提示框，同意或者或略。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
typedef struct tongyi
{
    uint32_t uid;
    GtkWidget *win;
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


int Friend_Fequest_Popup(uint32_t uid, const char *verification_message)
{
    GtkEventBox *popup_accept_eventbox, *popup_cancel_eventbox, *pop_mov_event;
    GtkWidget *popupwindow, *popupframelayout, *popuplayout;
    cairo_surface_t *popupsurfacecancel, *popupsurfacedone;
    cairo_surface_t *popupsurfacebackground;

    GtkWidget *popupcancel, *popupdone, *popupbackground;


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

    tongyi *info = malloc(sizeof(struct tongyi));
    info->uid = uid;
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


    GtkTextView *text, *yanzheng;
    // GtkWidget *text,*yanzheng;
    // text = gtk_text_view_new();
    yanzheng = gtk_text_view_new();
    char mes[80];
    sprintf(mes, "用户%d请求添加你为好友", uid);
    // gtk_test_text_set(text, mes);
    text = gtk_label_new(mes);
    yanzheng = gtk_label_new(verification_message);
//    sprintf(mes, "系统消息");
    //   gtk_test_text_set(yanzheng, verification_message);

//    GdkRGBA rgba = {0.92, 0.88, 0.74, 1};
//    gtk_widget_override_background_color(text, GTK_STATE_NORMAL, &rgba);//设置透明
    //gtk_widget_override_background_color(title, GTK_STATE_NORMAL, &rgba);//设置透明

    gtk_fixed_put(GTK_FIXED(popuplayout), GTK_WIDGET(text), 30, 70);
    gtk_fixed_put(GTK_FIXED(popuplayout), GTK_WIDGET(yanzheng), 40, 120);


    gtk_fixed_put(GTK_FIXED(popuplayout), GTK_WIDGET(popup_cancel_eventbox), 30, 170);
    gtk_fixed_put(GTK_FIXED(popuplayout), GTK_WIDGET(popup_accept_eventbox), 150, 170);


    gtk_container_add(GTK_CONTAINER (popupwindow), popupframelayout);
    gtk_container_add(GTK_CONTAINER (popupframelayout), popuplayout);

    gtk_widget_show_all(popupwindow);


    return 0;
}
