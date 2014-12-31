#include <gtk/gtk.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/md5.h>
#include <logger.h>
#include <protocol/login/Register.h>
#include <protocol/CRPPackets.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "newuser.h"
#include "client.h"
#include "PopupWinds.h"

GtkWidget *newwindow;
GtkWidget *zhuceLayout;
GtkWidget *mnickname, *username, *passwd1, *passwd2;
GtkWidget *background, *headline, *nickid, *nick, *nickmm1, *nickmm2, *mminfo, *endwind;
cairo_surface_t *surface1, *surface2, *surface3, *surface32, *surface33, *surface4, *surface5, *surface6, *surface7, *surface8, *surface82, *surface83;
int mx = 0;
int my = 0;

int newsockfd() {  //注册按钮点击事件
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
            .sin_family=AF_INET,
            .sin_addr.s_addr=htonl(INADDR_LOOPBACK),
            .sin_port=htons(8014)
    };
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        perror("Connect");
        return 0;
    }
    const gchar *newname, *newpwd, *newpwd2, *newnick;
    newname = gtk_entry_get_text(GTK_ENTRY(username));
    newpwd = gtk_entry_get_text(GTK_ENTRY(passwd1));
    newpwd2 = gtk_entry_get_text(GTK_ENTRY(passwd2));
    newnick = gtk_entry_get_text(GTK_ENTRY(mnickname));
    if ((strlen(newname) != 0) && (strlen(newpwd) != 0) && (strlen(newpwd2) != 0) && (strlen(newnick) != 0)) {
        int charnum;
        for (charnum = 0; newname[charnum];) {
            if ((isalnum(newname[charnum]) != 0) || (newname[charnum] == '@')
                    || (newname[charnum] == '.') || (newname[charnum] == '-') || (newname[charnum] == '_')) {
                charnum++;
            }
            else {
                break;
            }
        }
        if (charnum == strlen(newname)) {
            if (g_strcmp0(newpwd, newpwd2) != 0) {
                log_info("密码不一致", "密码不一致\n");
                popup("莫默告诉你：", "密码不一致", GTK_WINDOW(newwindow));
                return 1;
            }
            log_info("Hello", "Sending Hello\n");
            CRPHelloSend(sockfd, 0, 1, 1, 1);
            CRPBaseHeader *header;
            log_info("Hello", "Waiting OK\n");
            header = CRPRecv(sockfd);
            if (header->packetID != CRP_PACKET_OK) {
                log_error("Hello", "Recv Packet:%d\n", header->packetID);
                return 1;
            }
            unsigned char hash[16];
            MD5((unsigned char *) newpwd, 1, hash);
            CRPLoginRegisterSend(sockfd, 0, newname, hash, newnick);
            log_info("注册ing", "momo\n");
            header = CRPRecv(sockfd);
    asdasd;
            if (header->packetID != CRP_PACKET_OK) {
                log_error("Hello", "Recv Packet:%d\n", header->packetID);
                popup("莫默告诉你：", "登录名已存在", GTK_WINDOW(newwindow));
                return 1;
            }
            log_info("注册OK", "momo\n");
            popup("莫默告诉你：", "注册成功", GTK_WINDOW(newwindow));
            free(header);
        }
        else {
            log_info("不合格字符", "momo\n");
            popup("莫默告诉你：", "包含不合格字符", GTK_WINDOW(newwindow));
        }
    }
    else {
        log_info("注册信息不完整", "momo\n");
        popup("莫默告诉你：", "请完善注册信息", GTK_WINDOW(newwindow));
        return 1;
    }
    return 1;
}

static void create_zhucefaces() {

    surface1 = cairo_image_surface_create_from_png("注册背景.png");
    surface2 = cairo_image_surface_create_from_png("注册标题.png");

    surface3 = cairo_image_surface_create_from_png("注册按钮.png");
    surface32 = cairo_image_surface_create_from_png("注册按钮3.png");
    surface33 = cairo_image_surface_create_from_png("注册按钮2.png");

    surface4 = cairo_image_surface_create_from_png("资料1.png");
    surface5 = cairo_image_surface_create_from_png("资料2.png");
    surface6 = cairo_image_surface_create_from_png("资料3.png");
    surface7 = cairo_image_surface_create_from_png("资料4.png");

    surface8 = cairo_image_surface_create_from_png("关闭按钮1.png");
    surface82 = cairo_image_surface_create_from_png("关闭按钮2.png");
    surface83 = cairo_image_surface_create_from_png("关闭按钮3.png");

    background = gtk_image_new_from_surface(surface1);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), background, 0, 0);//起始坐标

    headline = gtk_image_new_from_surface(surface2);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), headline, 7, 10);

    mminfo = gtk_image_new_from_surface(surface3);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), mminfo, 20, 440);

    nickid = gtk_image_new_from_surface(surface4);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), nickid, 10, 80);

    nick = gtk_image_new_from_surface(surface5);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), nick, 10, 150);

    nickmm1 = gtk_image_new_from_surface(surface6);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), nickmm1, 10, 215);

    nickmm2 = gtk_image_new_from_surface(surface7);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), nickmm2, 10, 290);

    endwind = gtk_image_new_from_surface(surface8);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), endwind, 530, 0);

}

static void
destroy_surfaces() {
    g_print("destroying surfaces2");

    cairo_surface_destroy(surface1);
    cairo_surface_destroy(surface2);
    cairo_surface_destroy(surface3);
    cairo_surface_destroy(surface32);
    cairo_surface_destroy(surface33);
    cairo_surface_destroy(surface4);
    cairo_surface_destroy(surface5);
    cairo_surface_destroy(surface6);
    cairo_surface_destroy(surface7);
    cairo_surface_destroy(surface8);
    cairo_surface_destroy(surface82);
    cairo_surface_destroy(surface83);
}

//鼠标点击事件
static gint button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    mx = event->x;  // 取得鼠标相对于窗口的位置
    my = event->y;

    if (event->button == 1 && (mx > 530 && mx < 570) && (my > 0 && my < 24)) {              //设置关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) endwind, surface82); //置换图标
    }
    else if (event->button == 1 && (mx > 20 && mx < 160) && (my > 440 && my < 480)) {        //设置注册按钮
        gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) mminfo, surface32); //置换图标

    }
    else {
        gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_ARROW));
        if (event->button == 1) { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
            gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                    event->x_root, event->y_root, event->time);
        }
    }
    return 0;
}

//鼠标抬起事件
static gint button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    mx = event->x;  // 取得鼠标相对于窗口的位置
    my = event->y;

    if (event->button == 1 && (mx > 20 && mx < 160) && (my > 440 && my < 480)) {
        newsockfd();
    }
    else if (event->button == 1)       // 判断是否是点击关闭图标
    {
        gtk_image_set_from_surface((GtkImage *) endwind, surface83);  //设置关闭按钮
        if ((mx > 530 && mx < 570) && (my > 0 && my < 24)) {
            destroy_surfaces();
            gtk_widget_destroy(newwindow);
            //gtk_main_quit();
        }
    }
    return 0;
}

//鼠标移动事件
static gint motion_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    mx = event->x;  // 取得鼠标相对于窗口的位置
    my = event->y;
    if ((mx > 530 && mx < 570) && (my > 0 && my < 24)) {
        gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_HAND2));
        gtk_image_set_from_surface((GtkImage *) endwind, surface83);
    }
    else if (event->button == 1 && (mx > 20 && mx < 160) && (my > 440 && my < 480)) {
        gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_HAND2));
        gtk_image_set_from_surface((GtkImage *) mminfo, surface33);

    } else {
        gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_ARROW));
        gtk_image_set_from_surface((GtkImage *) endwind, surface8);
        gtk_image_set_from_surface((GtkImage *) mminfo, surface3);
    }
    return 0;
}

int newface() {

    newwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
    //gtk_window_set_default_size(GTK_WINDOW(newwindow), 570, 500);
    //gtk_window_set_position(GTK_WINDOW(newwindow), GTK_WIN_POS_MOUSE);
    gtk_window_set_position(GTK_WINDOW(newwindow), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (newwindow), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(newwindow), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(newwindow), 570, 500);

    zhuceLayout = gtk_fixed_new();
    create_zhucefaces();

    gtk_container_add(GTK_CONTAINER(newwindow), zhuceLayout);

    mnickname = gtk_entry_new();//昵称
    username = gtk_entry_new();//id
    passwd1 = gtk_entry_new();//密码1
    passwd2 = gtk_entry_new();//密码2

    gtk_entry_set_visibility(GTK_ENTRY(passwd1), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(passwd1), '*');
    gtk_entry_set_visibility(GTK_ENTRY(passwd2), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(passwd2), '*');

    gtk_fixed_put(GTK_FIXED(zhuceLayout), username, 100, 120);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), mnickname, 100, 180);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), passwd1, 100, 255);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), passwd2, 100, 326);

    // 设置窗体获取鼠标事件
    gtk_widget_set_events(newwindow,

            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK

                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK

                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

    g_signal_connect(G_OBJECT(newwindow), "button_press_event",
            G_CALLBACK(button_press_event), newwindow);       // 加入事件回调

    g_signal_connect(G_OBJECT(newwindow), "motion_notify_event",
            G_CALLBACK(motion_notify_event), newwindow);

    g_signal_connect(G_OBJECT(newwindow), "button_release_event",
            G_CALLBACK(button_release_event), newwindow);

    gtk_widget_show_all(newwindow);
}