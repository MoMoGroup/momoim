#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ClientSockfd.h"
#include "MainInterface.h"
#include "newuser.h"
#include "PopupWinds.h"
#include "common.h"

static GtkWidget *ssun,*imagelandbut, *imageregistered, *imageclosebut, *imagecancel;
GtkWidget *username, *passwd;
const gchar *name, *pwd;
static pthread_t thread1;
static int nX = 0, nY = 0;
static GtkWidget *window;

static cairo_surface_t *sbackground, *sheadimage, *swhite, *slandbut1, *slandbut2, *slandbut3, *saccount, *spasswd;
static cairo_surface_t *sregistered1, *sregistered2, *sclosebut1, *sclosebut2, *sclosebut3, *slandimage, *scancel10_1, *scancel10_2, *scancel10_3;
static GtkWidget *loginLayout, *pendingLayout, *frameLayout;
static GtkEventBox *sunevent_box,*landbutevent_box, *registeredevent_box, *closebutevent_box, *cancelevent_box, *backgroundevent_box, *waitevent_box;

gboolean mythread(gpointer user_data)//合并
{
    gtk_widget_destroy(window);
    maininterface();
    return 0;
}//合并

gboolean DestroyLayout(gpointer user_data)
{
    gtk_widget_hide(pendingLayout);
    gtk_widget_show_all(loginLayout);
    popup("莫默告诉你：", user_data);
    free(user_data);
    return FALSE;
}

//窗口
static void
create_surfaces1()
{
    sbackground = cairo_image_surface_create_from_png("背景2.png");
    ssun = gtk_image_new_from_file("1.gif");
    sheadimage = cairo_image_surface_create_from_png("头像.png");
    swhite = cairo_image_surface_create_from_png("白色.png");
    slandbut1 = cairo_image_surface_create_from_png("登陆按钮.png");
    slandbut2 = cairo_image_surface_create_from_png("登陆按钮2.png");
    slandbut3 = cairo_image_surface_create_from_png("登陆按钮3.png");
    saccount = cairo_image_surface_create_from_png("账号.png");
    spasswd = cairo_image_surface_create_from_png("密码.png");
    sregistered1 = cairo_image_surface_create_from_png("注册账号.png");
    sregistered2 = cairo_image_surface_create_from_png("注册账号2.png");
    sclosebut1 = cairo_image_surface_create_from_png("关闭按钮1.png");
    sclosebut2 = cairo_image_surface_create_from_png("关闭按钮2.png");
    sclosebut3 = cairo_image_surface_create_from_png("关闭按钮3.png");
    slandimage = cairo_image_surface_create_from_png("登录.png");
    scancel10_1 = cairo_image_surface_create_from_png("取消1.png");
    scancel10_2 = cairo_image_surface_create_from_png("取消2.png");
    scancel10_3 = cairo_image_surface_create_from_png("取消3.png");

}


static void
destroy_surfaces()
{
    g_print("destroying surfaces1");
    cairo_surface_destroy(sbackground);
    cairo_surface_destroy(sheadimage);
    cairo_surface_destroy(swhite);
    cairo_surface_destroy(slandbut1);
    cairo_surface_destroy(slandbut2);
    cairo_surface_destroy(slandbut3);
    cairo_surface_destroy(saccount);
    cairo_surface_destroy(spasswd);
    cairo_surface_destroy(sregistered1);
    cairo_surface_destroy(sregistered2);
    cairo_surface_destroy(sclosebut1);
    cairo_surface_destroy(sclosebut2);
    cairo_surface_destroy(sclosebut3);
    cairo_surface_destroy(slandimage);
    cairo_surface_destroy(scancel10_1);
    cairo_surface_destroy(scancel10_2);
    cairo_surface_destroy(scancel10_3);
}

extern int DeleteEvent()
{
    gtk_main_quit();
    return TRUE;
}

void *sendhello(void *M)
{
    mysockfd();
    return 0;
}

void on_button_clicked()
{

    name = gtk_entry_get_text(GTK_ENTRY(username));
    pwd = gtk_entry_get_text(GTK_ENTRY(passwd));
    if ((strlen(name) != 0) && (strlen(pwd) != 0))
    {
        int charnum, number = 0;
        for (charnum = 0; name[charnum];)
        {
            if ((isalnum(name[charnum]) != 0) || (name[charnum] == '@')
                    || (name[charnum] == '.') || (name[charnum] == '-') || (name[charnum] == '_'))
            {
                if (isdigit(name[charnum]) != 0)
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
        if (charnum == strlen(name))
        {
        }
        else
        {
            popup("莫默告诉你：", "包含不合格字符");
            return;
        }
    }
    else
    {
        popup("莫默告诉你：", "请填写登录信息");
        return;
    }
    gtk_widget_hide(loginLayout);//隐藏loginlayout
    //gtk_widget_destroy(layout);销毁layout对话框

    gtk_container_add(GTK_CONTAINER (frameLayout), pendingLayout);

    gtk_widget_show_all(pendingLayout);//显示layout2

    pthread_create(&thread1, NULL, sendhello, NULL);

}

static gint background_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    //设置在非按钮区域内移动窗口
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                event->x_root, event->y_root, event->time);
    }
    return 0;

}

static gint wait_button_press_event(GtkWidget *widget,   //第二界面的窗体移动

        GdkEventButton *event, gpointer data)
{
    //设置在非按钮区域内移动窗口
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                event->x_root, event->y_root, event->time);
    }
    return 0;
}

static gint landbut_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{

    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut2);
    }

    return 0;

}

// 鼠标抬起事件
static gint landbut_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    if (event->button == 1)  //判断是否在登陆区域中，设置登陆按钮
    {
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
        gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut1);
        on_button_clicked();
    }
    return 0;
}

static gint landbut_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 鼠标移动事件

{

    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut3);
    return 0;
}

static gint landbut_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 鼠标移动事件

{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut1);
    return 0;
}

static gint registered_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        // 设置注册按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) imageregistered, sregistered2);

    }
    return 0;

}

// 鼠标抬起事件
static gint registered_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    //info();
    newface(); //调用注册界面
    return 0;
}

static gint registered_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 鼠标移动事件

{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
    return 0;
}

static gint registered_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 离开事件

{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    return 0;
}

static gint closebut_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{

    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {              //设置关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) imageclosebut, sclosebut2); //置换图标
    }
    return 0;

}

// 鼠标抬起事件
static gint closebut_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    if (event->button == 1)       // 判断是否是点击关闭图标
    {
        gtk_image_set_from_surface((GtkImage *) imageclosebut, sclosebut1);  //设置关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));

        DeleteEvent();
    }
    return 0;
}

static gint closebut_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 鼠标移动事件

{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) imageclosebut, sclosebut3);
    return 0;
}

static gint closebut_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 鼠标移动事件

{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) imageclosebut, sclosebut1);
    return 0;
}


static gint cancel_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {   //设置第二界面取消按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
        gtk_image_set_from_surface((GtkImage *) imagecancel, scancel10_3);//设置鼠标光标
    }

    return 0;
}

// 鼠标抬起事件
static gint cancel_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    if (event->button == 1)
    {                                         //设置取消按钮
        gtk_image_set_from_surface((GtkImage *) imagecancel, scancel10_1);
        pthread_cancel(thread1);
        CRPClose(sockfd);
        gtk_widget_hide(pendingLayout);
        gtk_widget_show_all(loginLayout);
    }

    return 0;
}

static gint cancel_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 鼠标移动事件

{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) imagecancel, scancel10_2);
    return 0;
}

static gint cancel_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)         // 鼠标离开事件

{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) imagecancel, scancel10_1);
    return 0;
}


int main(int argc, char *argv[])
{


    //初始化GTK+程序
    gtk_init(&argc, &argv);
    //创建窗口，并为窗口的关闭信号加回调函数以便退出

    loadloginLayout("ad");//加载登陆界面

    gtk_main();
    destroy_surfaces();
    return 0;
}

gboolean destoryall(gpointer user_data)
{
    g_idle_add(DestoryMainInterFace, NULL);//销毁主窗口,--maininterface

    friendinfo *head = friendinfohead;
    friendinfo *p;
    while (head->next)
    {
        p = head->next;
        head->next = p->next;
        if (p->chartwindow)
        {
            gtk_widget_destroy(p->chartwindow);
        }
        free(p);
    }
    g_idle_add(loadloginLayout, NULL);
    popup("异地登录","您的帐号在别处登录，\n 如非本人操作，\n请尽快修改密码");
    return FALSE;
}


gboolean loadloginLayout(gpointer user_data)
{

    //加载loginlayout

    create_surfaces1();
    GtkWidget *imagebackground, *imagehead, *imagewhite, *imageaccount, *imagepasswd;
    GtkWidget *iwait, *imainland;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(gtk_main_quit), NULL);


    //gtk_window_set_default_size(GTK_WINDOW(window), 283, 411);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);//窗口出现位置
    gtk_window_set_resizable(GTK_WINDOW (window), FALSE);//窗口不可改变
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);   // 去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(window), 283, 411);

    imagebackground = gtk_image_new_from_surface(sbackground);
    imagehead = gtk_image_new_from_surface(sheadimage);
    imagewhite = gtk_image_new_from_surface(swhite);
    imagelandbut = gtk_image_new_from_surface(slandbut1);
    imageaccount = gtk_image_new_from_surface(saccount);
    imagepasswd = gtk_image_new_from_surface(spasswd);
    imageregistered = gtk_image_new_from_surface(sregistered1);
    imageclosebut = gtk_image_new_from_surface(sclosebut1);
    iwait = gtk_image_new_from_file("等待.gif");
    imainland = gtk_image_new_from_surface(slandimage);
    imagecancel = gtk_image_new_from_surface(scancel10_1);

//    gtk_container_add (GTK_CONTAINER(loginLayout),landbutevent_box);
//    gtk_container_add (GTK_CONTAINER(loginLayout), registeredevent_box);
//    gtk_container_add (GTK_CONTAINER(loginLayout), closebutevent_box);
//    gtk_container_add (GTK_CONTAINER(loginLayout),cancelevent_box);


    backgroundevent_box = BuildEventBox(
            imagebackground,
            G_CALLBACK(background_button_press_event),
            NULL, NULL, NULL, NULL);

    waitevent_box = BuildEventBox(iwait,
            G_CALLBACK(wait_button_press_event),
            NULL, NULL, NULL, NULL);

    landbutevent_box = BuildEventBox(
            imagelandbut,
            G_CALLBACK(landbut_button_press_event),
            G_CALLBACK(landbut_enter_notify_event),
            G_CALLBACK(landbut_leave_notify_event),
            G_CALLBACK(landbut_button_release_event),
            NULL
    );

    sunevent_box = BuildEventBox(
            ssun,
            G_CALLBACK(background_button_press_event),
            NULL,
            NULL,
            NULL,
            NULL);

    registeredevent_box = BuildEventBox(
            imageregistered,
            G_CALLBACK(registered_button_press_event),
            G_CALLBACK(registered_enter_notify_event),
            G_CALLBACK(registered_leave_notify_event),
            G_CALLBACK(registered_button_release_event),
            NULL);
    closebutevent_box = BuildEventBox(
            imageclosebut,
            G_CALLBACK(closebut_button_press_event),
            G_CALLBACK(closebut_enter_notify_event),
            G_CALLBACK(closebut_leave_notify_event),
            G_CALLBACK(closebut_button_release_event),
            NULL);
    cancelevent_box = BuildEventBox(
            imagecancel,
            G_CALLBACK(cancel_button_press_event),
            G_CALLBACK(cancel_enter_notify_event),
            G_CALLBACK(cancel_leave_notify_event),
            G_CALLBACK(cancel_button_release_event),
            NULL);

    frameLayout = gtk_layout_new(NULL, NULL);
    pendingLayout = gtk_fixed_new();
    loginLayout = gtk_fixed_new();

    gtk_fixed_put(GTK_FIXED(loginLayout), backgroundevent_box, 0, 0);//起始坐标
    gtk_fixed_put(GTK_FIXED(loginLayout), imagehead, 61, 30);
    gtk_fixed_put(GTK_FIXED(loginLayout), imagewhite, 25, 200);
    gtk_fixed_put(GTK_FIXED(loginLayout), landbutevent_box, 75, 300);
    gtk_fixed_put(GTK_FIXED(loginLayout), imageaccount, 35, 220);
    gtk_fixed_put(GTK_FIXED(loginLayout), imagepasswd, 35, 260);
    gtk_fixed_put(GTK_FIXED(loginLayout), registeredevent_box, 5, 380);
    gtk_fixed_put(GTK_FIXED(loginLayout), closebutevent_box, 247, 0);
    gtk_fixed_put(GTK_FIXED(loginLayout), sunevent_box, 3, 3);
    gtk_fixed_put(GTK_FIXED(pendingLayout), imainland, 0, 0);
    gtk_fixed_put(GTK_FIXED(pendingLayout), waitevent_box, 55, 110);
    gtk_fixed_put(GTK_FIXED(pendingLayout), cancelevent_box, 75, 350);

    gtk_container_add(GTK_CONTAINER (window), frameLayout);//frameLayout 加入到window
    gtk_container_add(GTK_CONTAINER (frameLayout), loginLayout);

    username = gtk_entry_new();
    passwd = gtk_entry_new();
    gtk_entry_set_max_length(username, 20);//设置输入的最大长度
    gtk_entry_set_max_length(passwd, 20);

    gtk_test_text_set(username, "a");
    gtk_test_text_set(passwd, "a");

    gtk_entry_set_visibility(GTK_ENTRY(passwd), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(passwd), '*');

    gtk_fixed_put(GTK_FIXED(loginLayout), username, 85, 220);
    gtk_fixed_put(GTK_FIXED(loginLayout), passwd, 85, 260);

    gtk_widget_show(landbutevent_box);
    gtk_widget_show_all(window);
    return FALSE;
}