#include <gtk/gtk.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/md5.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "newuser.h"
#include "PopupWinds.h"
#include "common.h"

static GtkWidget *newwindow;
static GtkWidget *zhuceLayout;
static GtkWidget *mnickname, *newusername, *passwd1, *passwd2;
static GtkWidget *background, *headline, *nickid, *nick, *nickmm1, *nickmm2, *mminfo, *endwind;
static cairo_surface_t *surface1, *surface2, *surface3, *surface32, *surface33, *surface4, *surface5, *surface6, *surface7, *surface8, *surface82, *surface83;
static GtkEventBox *closebut_event_box, *zhuce_event_box, *newbackground_event_box;

int newsockfd()
{
    //注册按钮点击事件
    const gchar *newname, *newpwd, *newpwd2, *newnick;
    newname = gtk_entry_get_text(GTK_ENTRY(newusername));
    newpwd = gtk_entry_get_text(GTK_ENTRY(passwd1));
    newpwd2 = gtk_entry_get_text(GTK_ENTRY(passwd2));
    newnick = gtk_entry_get_text(GTK_ENTRY(mnickname));

    if ((strlen(newname) != 0) && (strlen(newpwd) != 0) && (strlen(newpwd2) != 0) && (strlen(newnick) != 0))
    {
        int charnum, number = 0;
        for (charnum = 0; newname[charnum];)
        {

            if ((isalnum(newname[charnum]) != 0) || (newname[charnum] == '@')
                                                    || (newname[charnum] == '.') || (newname[charnum] == '-') || (newname[charnum] == '_'))
            {
                if (isdigit(newname[charnum]) != 0)
                {
                    number++;
                }
                charnum++;
            }
            else
            {
                break;
            }
        }
        if (charnum == strlen(newname))
        {
            int fd = socket(AF_INET, SOCK_STREAM, 0);
            if (number == charnum)
            {
                log_info("登录名全为数字", "登录名全为数字\n");
                popup("莫默告诉你：", "登录名全为数字");
                return 1;
            }
            if (g_strcmp0(newpwd, newpwd2) != 0)
            {
                log_info("密码不一致", "密码不一致\n");
                popup("莫默告诉你：", "两次密码不一致");
                return 1;
            }
            struct sockaddr_in server_addr = {
                    .sin_family=AF_INET,
                    .sin_addr.s_addr=htonl(INADDR_LOOPBACK),
                    .sin_port=htons(8014)
            };
            if (connect(fd, (struct sockaddr *) &server_addr, sizeof(server_addr)))
            {
                perror("Connect");
                return 0;
            }
            CRPContext sockfd = CRPOpen(fd);
            log_info("Hello", "Sending Hello\n");
            CRPHelloSend(sockfd, 0, 1, 1, 1, 0);
            CRPBaseHeader *header;
            log_info("Hello", "Waiting OK\n");
            header = CRPRecv(sockfd);
            if (header->packetID != CRP_PACKET_OK)
            {
                log_error("Hello", "Recv Packet:%d\n", header->packetID);
                return 1;
            }
            unsigned char hash[16];
            MD5((unsigned char *) newpwd, strlen(newpwd), hash);
            CRPLoginRegisterSend(sockfd, 0, newname, hash, newnick);
            log_info("注册ing", "momo\n");
            header = CRPRecv(sockfd);

            if (header->packetID != CRP_PACKET_OK)
            {
                log_error("Hello", "Recv Packet:%d\n", header->packetID);
                popup("莫默告诉你：", "登录名已经存在");
                return 1;
            }
            log_info("注册OK", "momo\n");
            popup("莫默告诉你：", "欢迎你加入莫默");
            free(header);
            //destroy_surfaces();
            gtk_widget_destroy(newwindow);
            CRPClose(sockfd);
        }
        else
        {
            log_info("不合格字符", "momo\n");
            popup("莫默告诉你：", "包含不合格字符");
            return 1;
        }
    }
    else
    {
        log_info("注册信息不完整", "momo\n");
        popup("莫默告诉你：", "请完善注册信息");
        return 1;
    }
}

static void create_zhucefaces()
{

    surface1 = cairo_image_surface_create_from_png("注册背景1.png");
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
}

static void
destroy_surfaces()
{
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

//背景的eventbox拖曳窗口
static gint newbackground_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                event->x_root, event->y_root, event->time);
    }
    return 0;
}

//注册
//鼠标点击事件
static gint zhuce_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{


    if (event->button == 1)
    {        //设置注册按钮
        gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) mminfo, surface32); //置换图标
    }
    return 0;
}

//注册
//鼠标抬起事件
static gint zhuce_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    if (event->button == 1)
    {
        newsockfd();
    }

    return 0;
}

//注册
//鼠标移动事件
static gint zhuce_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) mminfo, surface33);
    return 0;
}

//鼠标离开事件
static gint zhuce_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) mminfo, surface3);

    return 0;
}

//关闭按钮
//鼠标点击事件
static gint closebut_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {              //设置关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) endwind, surface82); //置换图标
    }

    return 0;
}

//关闭按钮
//鼠标抬起事件
static gint closebut_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)       // 判断是否是点击关闭图标
    {
        gtk_image_set_from_surface((GtkImage *) endwind, surface8);  //设置关闭按钮
        destroy_surfaces();
        gtk_widget_destroy(newwindow);
        //gtk_main_quit();
    }
    return 0;
}

//关闭按钮
//鼠标移动事件
static gint closebut_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) endwind, surface83);

    return 0;
}

//鼠标离开事件
static gint closebut_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) endwind, surface8);
    gtk_image_set_from_surface((GtkImage *) mminfo, surface3);
    return 0;
}

int newface()
{
    newwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(newwindow), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (newwindow), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(newwindow), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(newwindow), 500, 500);

    zhuceLayout = gtk_fixed_new();
    create_zhucefaces();
    gtk_container_add(GTK_CONTAINER(newwindow), zhuceLayout);
    background = gtk_image_new_from_surface(surface1);
    headline = gtk_image_new_from_surface(surface2);
    mminfo = gtk_image_new_from_surface(surface3);
    nickid = gtk_image_new_from_surface(surface4);
    nick = gtk_image_new_from_surface(surface5);
    nickmm1 = gtk_image_new_from_surface(surface6);
    nickmm2 = gtk_image_new_from_surface(surface7);
    endwind = gtk_image_new_from_surface(surface8);

    newbackground_event_box = BuildEventBox(
            background,
            G_CALLBACK(newbackground_button_press_event),
            NULL, NULL, NULL, NULL);

    zhuce_event_box = BuildEventBox(
            mminfo,
            G_CALLBACK(zhuce_button_press_event),
            G_CALLBACK(zhuce_enter_notify_event),
            G_CALLBACK(zhuce_leave_notify_event),
            G_CALLBACK(zhuce_button_release_event),
            NULL);

    closebut_event_box = BuildEventBox(
            endwind,
            G_CALLBACK(closebut_button_press_event),
            G_CALLBACK(closebut_enter_notify_event),
            G_CALLBACK(closebut_leave_notify_event),
            G_CALLBACK(closebut_button_release_event),
            NULL);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), newbackground_event_box, 0, 0);//起始坐标
    gtk_widget_set_size_request(GTK_WIDGET(background), 500, 500);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), zhuce_event_box, 20, 440);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), closebut_event_box, 530, 0);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), headline, 7, 10);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), nickid, 10, 80);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), nick, 10, 150);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), nickmm1, 10, 215);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), nickmm2, 10, 290);





    mnickname = gtk_entry_new();//昵称
    newusername = gtk_entry_new();//id
    passwd1 = gtk_entry_new();//密码1
    passwd2 = gtk_entry_new();//密码2

    gtk_entry_set_visibility(GTK_ENTRY(passwd1), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(passwd1), '*');
    gtk_entry_set_visibility(GTK_ENTRY(passwd2), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(passwd2), '*');

    gtk_fixed_put(GTK_FIXED(zhuceLayout), newusername, 100, 120);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), mnickname, 100, 180);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), passwd1, 100, 255);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), passwd2, 100, 326);

    gtk_widget_show_all(newwindow);
}