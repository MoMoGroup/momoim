#include <gtk/gtk.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/md5.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include "newuser.h"
#include "PopupWinds.h"
#include "common.h"
#include "client.h"

/*注册新用户界面*/
static GtkWidget *NewWindow;
static GtkWidget *ZhuceLayout;
static GtkWidget *MoNickname, *NewUsername, *Passwd1, *Passwd2;
static GtkWidget *BackGround1, *MoInfo, *EndWind;
static cairo_surface_t *surface1, *surface3, *surface32, *surface33, *surface8, *surface82, *surface83;
static GtkEventBox *closebut_event_box, *zhuce_event_box, *newbackground_event_box;

int newsockfd()
{
    //注册按钮点击事件
    const gchar *newname, *newpwd, *newpwd2, *newnick;
    /*获取输入框中的输入*/
    newname = gtk_entry_get_text(GTK_ENTRY(NewUsername));
    newpwd = gtk_entry_get_text(GTK_ENTRY(Passwd1));
    newpwd2 = gtk_entry_get_text(GTK_ENTRY(Passwd2));
    newnick = gtk_entry_get_text(GTK_ENTRY(MoNickname));

    /*输入的字符传长度不为0时*/
    if ((strlen(newname) != 0) && (strlen(newpwd) != 0) && (strlen(newpwd2) != 0) && (strlen(newnick) != 0))
    {
        int charnum, number = 0;
        for (charnum = 0; newname[charnum];)//循环判断用户名中是否包含非法字符
        {
            if ((isalnum(newname[charnum]) != 0) || (newname[charnum] == '@')
                                                    || (newname[charnum] == '.') || (newname[charnum] == '-') || (newname[charnum] == '_'))
            {
                //不为上面五种情况下判断含数字的多少
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
            if (number == charnum)//比较含数字的数量是否跟字符串长度相等
            {
                log_info("登录名全为数字", "登录名全为数字\n");
                popup("莫默告诉你：", "登录名全为数字");
                return 1;
            }
            if (g_strcmp0(newpwd, newpwd2) != 0)//比较两次密码是否相同
            {
                log_info("密码不一致", "密码不一致\n");
                popup("莫默告诉你：", "两次密码不一致");
                return 1;
            }
            int fd = socket(AF_INET, SOCK_STREAM, 0);
            FILE *ipfp2;
            char myip1[20];
            struct in_addr inp;
            ipfp2 = fopen(checkmulu_ip, "r");//checkmulu_ip为全局变量的文件名，存放首次启动时输入的IP地址
            size_t n = fread(myip1, 1, 20, ipfp2);
            myip1[n] = 0;
            log_info("IP", myip1);
            inet_aton(myip1, &inp);//字符串IP地址转换为一个32位的网络序列IP地址
            struct sockaddr_in server_addr = {
                    .sin_family=AF_INET,
                    //.sin_addr.s_addr=htonl(INADDR_LOOPBACK),
                    .sin_addr.s_addr=inp.s_addr,
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
            if (header == NULL || header->packetID != CRP_PACKET_OK)
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
            gtk_widget_destroy(NewWindow);
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
    return 0;
}

static void create_zhucefaces()
{
    surface1 = ChangeThem_png("注册背景.png");

    surface3 = ChangeThem_png("注册按钮.png");
    surface32 = ChangeThem_png("注册按钮3.png");
    surface33 = ChangeThem_png("注册按钮2.png");

    surface8 = ChangeThem_png("关闭按钮1.png");
    surface82 = ChangeThem_png("关闭按钮2.png");
    surface83 = ChangeThem_png("关闭按钮3.png");
}

static void destroy_surfaces()
{
    g_print("destroying surfaces2");

    cairo_surface_destroy(surface1);
    cairo_surface_destroy(surface3);
    cairo_surface_destroy(surface32);
    cairo_surface_destroy(surface33);
    cairo_surface_destroy(surface8);
    cairo_surface_destroy(surface82);
    cairo_surface_destroy(surface83);
}

//背景的eventbox拖曳窗口
static gint newbackground_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(NewWindow), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   (gint) event->x_root, (gint) event->y_root, event->time);
    }
    return 0;
}

//注册
//鼠标点击事件
static gint zhuce_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {        //设置注册按钮
        gdk_window_set_cursor(gtk_widget_get_window(NewWindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) MoInfo, surface32); //置换图标
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
    gdk_window_set_cursor(gtk_widget_get_window(NewWindow), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) MoInfo, surface33);
    return 0;
}

//鼠标离开事件
static gint zhuce_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(NewWindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) MoInfo, surface3);

    return 0;
}

//关闭按钮
//鼠标点击事件
static gint closebut_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {              //设置关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(NewWindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) EndWind, surface82); //置换图标
    }

    return 0;
}

//关闭按钮
//鼠标抬起事件
static gint closebut_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)       // 判断是否是点击关闭图标
    {
        gtk_image_set_from_surface((GtkImage *) EndWind, surface8);  //设置关闭按钮
        destroy_surfaces();
        gtk_widget_destroy(NewWindow);
    }
    return 0;
}

//关闭按钮
//鼠标移动事件
static gint closebut_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(NewWindow), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) EndWind, surface83);

    return 0;
}

//鼠标离开事件
static gint closebut_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(NewWindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) EndWind, surface8);
    gtk_image_set_from_surface((GtkImage *) MoInfo, surface3);
    return 0;
}

int newface()
{
    NewWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);//创建新窗口
    gtk_window_set_position(GTK_WINDOW(NewWindow), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (NewWindow), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(NewWindow), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(NewWindow), 500, 500);//设置窗口大小

    ZhuceLayout = gtk_fixed_new();//新建layout布局容纳控件
    create_zhucefaces();
    gtk_container_add(GTK_CONTAINER(NewWindow), ZhuceLayout);//将layout的添加到窗体中
    BackGround1 = gtk_image_new_from_surface(surface1);//为控件加上特定的图片
    MoInfo = gtk_image_new_from_surface(surface3);
    EndWind = gtk_image_new_from_surface(surface8);

    newbackground_event_box = BuildEventBox(BackGround1,
                                            G_CALLBACK(newbackground_button_press_event),
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL);

    zhuce_event_box = BuildEventBox(MoInfo,
                                    G_CALLBACK(zhuce_button_press_event),
                                    G_CALLBACK(zhuce_enter_notify_event),
                                    G_CALLBACK(zhuce_leave_notify_event),
                                    G_CALLBACK(zhuce_button_release_event),
                                    NULL,
                                    NULL);

    closebut_event_box = BuildEventBox(EndWind,
                                       G_CALLBACK(closebut_button_press_event),
                                       G_CALLBACK(closebut_enter_notify_event),
                                       G_CALLBACK(closebut_leave_notify_event),
                                       G_CALLBACK(closebut_button_release_event),
                                       NULL,
                                       NULL);
    gtk_fixed_put(GTK_FIXED(ZhuceLayout), (GtkWidget *) newbackground_event_box, 0, 0);//起始坐标
    gtk_widget_set_size_request(GTK_WIDGET(BackGround1), 500, 500);
    gtk_fixed_put(GTK_FIXED(ZhuceLayout), (GtkWidget *) zhuce_event_box, 20, 440);
    gtk_fixed_put(GTK_FIXED(ZhuceLayout), (GtkWidget *) closebut_event_box, 530, 0);

    MoNickname = gtk_entry_new();//昵称
    NewUsername = gtk_entry_new();//id
    Passwd1 = gtk_entry_new();//密码1
    Passwd2 = gtk_entry_new();//密码2
    gtk_entry_set_max_length((GTK_ENTRY(MoNickname)), 20);//设置输入的最大长度
    gtk_entry_set_max_length((GTK_ENTRY(NewUsername)), 20);
    gtk_entry_set_max_length((GTK_ENTRY(Passwd1)), 20);
    gtk_entry_set_max_length((GTK_ENTRY(Passwd2)), 20);

    gtk_entry_set_visibility(GTK_ENTRY(Passwd1), FALSE);//设置密码不可见
    gtk_entry_set_invisible_char(GTK_ENTRY(Passwd1), '*');//将不可见设置为*
    gtk_entry_set_visibility(GTK_ENTRY(Passwd2), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(Passwd2), '*');

    gtk_fixed_put(GTK_FIXED(ZhuceLayout), NewUsername, 100, 120);
    gtk_fixed_put(GTK_FIXED(ZhuceLayout), MoNickname, 100, 180);
    gtk_fixed_put(GTK_FIXED(ZhuceLayout), Passwd1, 100, 255);
    gtk_fixed_put(GTK_FIXED(ZhuceLayout), Passwd2, 100, 326);

    gtk_widget_show_all(NewWindow);
    return 0;
}