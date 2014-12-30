#include <gtk/gtk.h>
#include <stdlib.h>
#include "ClientSockfd.h"
#include "MainInterface.h"
#include "newuser.h"
#include <cairo.h>
#include <bits/sigthread.h>
#include <sys/socket.h>
#include <logger.h>


static GtkWidget *imagelandbut, *imageregistered, *imageclosebut, *imagecancel;
GtkWidget *username, *passwd;
static pthread_t thread1;
static int sockfd;
static int nX = 0,nY=0;
static GtkWidget *window;

static cairo_surface_t *sbackground, *sheadimage, *swhite, *slandbut1, *slandbut2, *slandbut3, *saccount, *spasswd;
static cairo_surface_t *sregistered1, *sregistered2, *sclosebut1, *sclosebut2, *sclosebut3,*slandimage, *scancel10_1, *scancel10_2, *scancel10_3;
static int login_hind_flag = 1;
static GtkWidget *loginLayout, *pendingLayout, *frameLayout;

gboolean mythread(gpointer user_data)//合并
{
    gtk_widget_destroy(window);
    maininterface();
    return 0;
}//合并

gboolean destroyLayout(gpointer user_data){
    login_hind_flag = 1;

    gtk_widget_destroy(pendingLayout);
    gtk_widget_show_all(loginLayout);
}
//关闭窗口
static void
create_surfaces1() {
    GtkWidget *imagebackground, *imagehead, *imagewhite, *imageaccount, *imagepasswd;
    GtkWidget *iwait, *imainland;

    sbackground = cairo_image_surface_create_from_png("背景.png");
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

    imagebackground = gtk_image_new_from_surface(sbackground);
    gtk_fixed_put(GTK_FIXED(loginLayout), imagebackground, 0, 0);//起始坐标

    imagehead = gtk_image_new_from_surface(sheadimage);
    gtk_fixed_put(GTK_FIXED(loginLayout), imagehead, 61, 30);

    imagewhite = gtk_image_new_from_surface(swhite);
    gtk_fixed_put(GTK_FIXED(loginLayout), imagewhite, 25, 200);

    imagelandbut = gtk_image_new_from_surface(slandbut1);
    gtk_fixed_put(GTK_FIXED(loginLayout), imagelandbut, 70, 300);

    imageaccount = gtk_image_new_from_surface(saccount);
    gtk_fixed_put(GTK_FIXED(loginLayout), imageaccount, 35, 220);

    imagepasswd= gtk_image_new_from_surface(spasswd);
    gtk_fixed_put(GTK_FIXED(loginLayout), imagepasswd, 35, 260);
    imageregistered = gtk_image_new_from_surface(sregistered1);
    gtk_fixed_put(GTK_FIXED(loginLayout), imageregistered, 5, 380);

    imageclosebut = gtk_image_new_from_surface(sclosebut1);
    gtk_fixed_put(GTK_FIXED(loginLayout), imageclosebut, 247, 0);

    iwait = gtk_image_new_from_file("等待.gif");
    gtk_fixed_put(GTK_FIXED(pendingLayout),iwait, 0, 0);

    imainland = gtk_image_new_from_surface(slandimage);
    gtk_fixed_put(GTK_FIXED(pendingLayout), imainland, 80, 20);

    imagecancel = gtk_image_new_from_surface(scancel10_1);
    gtk_fixed_put(GTK_FIXED(pendingLayout), imagecancel, 70, 310);
}


static void
destroy_surfaces() {
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

extern int DeleteEvent() {
    gtk_main_quit();
    return TRUE;
}

void *sendhello(void *M) {
    mysockfd();
    return 0;
}

void on_button_clicked() {


    gtk_widget_hide(loginLayout);//隐藏loginlayout
    login_hind_flag = 0;
    //gtk_widget_destroy(layout);销毁layout对话框


    gtk_container_add(GTK_CONTAINER (frameLayout), pendingLayout);

    gtk_widget_show_all(pendingLayout);//显示layout2

    pthread_create(&thread1, NULL, sendhello, NULL);


}

static gint button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data) {
    nX = event->x;  // 取得鼠标相对于窗口的位置
    nY = event->y;
    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        if (event->button == 1 && (nX > 75 && nX < 205) && (nY > 302 && nY < 335) && login_hind_flag == 1)    // 判断是否左键按下且在区域内,设置登陆按钮

        {
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
            gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut2);
        }
        else if (event->button == 1 && (nX > 247 && nX < 280) && (nY > 2 && nY < 25) && login_hind_flag == 1) {              //设置关闭按钮
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
            gtk_image_set_from_surface((GtkImage *) imageclosebut, sclosebut2); //置换图标
        }
        else if (event->button == 1 && (nX > 5 && nX < 62) && (nY > 380 && nY < 395) && login_hind_flag == 1)       // 判断是否左键按下

        {                                                                                           //设置注册按钮
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
            gtk_image_set_from_surface((GtkImage *) imageregistered, sregistered2);
            newface(); //调用注册界面
        }
        else if (event->button == 1 && (nX > 75 && nX < 202) && (nY > 312 && nY < 350) && login_hind_flag == 0) {   //设置第二界面取消按钮
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
            gtk_image_set_from_surface((GtkImage *) imagecancel, scancel10_3);//设置鼠标光标
        }
        else {                                                                               //设置在非按钮区域内移动窗口
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
            if (event->button == 1) { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
                gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                        event->x_root, event->y_root, event->time);
            }
        }
    }
    return 0;

}

// 鼠标抬起事件
static gint button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)

{

    nX = event->x;  // 取得鼠标相对于窗口的位置
    nY = event->y;
    if (event->button == 1 && (nX > 75 && nX < 205) && (nY > 302 && nY < 335) && login_hind_flag == 1)  //判断是否在登陆区域中，设置登陆按钮
    {
        gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut1);
        on_button_clicked();
    }
    else if (event->button == 1 && login_hind_flag == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) imageclosebut, sclosebut1);  //设置关闭按钮
        if ((nX > 247 && nX < 280) && (nY > 2 && nY < 25))
            DeleteEvent();
    }
    else if (login_hind_flag == 0) {                                         //设置取消按钮
        if (event->button == 1 && (nX > 75 && nX < 202) && (nY > 312 && nY < 355)) {
            gtk_image_set_from_surface((GtkImage *) imagecancel, scancel10_1);
            if ((nX > 75 && nX < 202) && (nY > 312 && nY < 355)) {
                close(sockfd);
                pthread_cancel(thread1);
                login_hind_flag = 1;
                gtk_widget_hide(pendingLayout);
                gtk_widget_show_all(loginLayout);
            }
        }
    }

    return 0;
}

static gint motion_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 鼠标移动事件

{
    nX = event->x;  // 取得鼠标相对于窗口的位置
    nY = event->y;
    if (login_hind_flag == 1) {
        if ((nX > 75 && nX < 205) && (nY > 302 && nY < 335)) {
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
            gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut3);
        }
        else if ((nX > 247 && nX < 280) && (nY > 2 && nY < 25)){
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
            gtk_image_set_from_surface((GtkImage *) imageclosebut, sclosebut3);
        }
        else if((nX > 5 && nX < 62) && (nY > 380 && nY < 395)) {
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
        }
        else{
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));

            gtk_image_set_from_surface((GtkImage *) imageclosebut, sclosebut1);
            gtk_image_set_from_surface((GtkImage *) imagelandbut, slandbut1);
        }
    }
    else if (login_hind_flag == 0) {
        if ((nX > 75 && nX < 202) && (nY > 312 && nY < 355)) {
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
            gtk_image_set_from_surface((GtkImage *) imagecancel, scancel10_2);
        }
        else {

            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
            gtk_image_set_from_surface((GtkImage *) imagecancel, scancel10_1);

        }
    }

    return 0;
}

int main(int argc, char *argv[]) {


    //初始化GTK+程序
    gtk_init(&argc, &argv);
    //创建窗口，并为窗口的关闭信号加回调函数以便退出
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(gtk_main_quit), NULL);

    //gtk_window_set_default_size(GTK_WINDOW(window), 283, 411);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);//窗口出现位置
    gtk_window_set_resizable(GTK_WINDOW (window), FALSE);//窗口不可改变
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);   // 去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(window), 283, 411);


    gtk_widget_set_events(window,  // 设置窗体获取鼠标事件

            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK

                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK

                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

    g_signal_connect(G_OBJECT(window), "button_press_event",
            G_CALLBACK(button_press_event), window);       // 加入事件回调

    g_signal_connect(G_OBJECT(window), "motion_notify_event",
            G_CALLBACK(motion_notify_event), window);

    g_signal_connect(G_OBJECT(window), "button_release_event",
            G_CALLBACK(button_release_event), window);
    pendingLayout = gtk_fixed_new();
    loginLayout = gtk_fixed_new();
    create_surfaces1();

    frameLayout = gtk_layout_new(NULL, NULL);

    gtk_container_add(GTK_CONTAINER (window), frameLayout);//frameLayout 加入到window
    gtk_container_add(GTK_CONTAINER (frameLayout), loginLayout);

    username = gtk_entry_new();
    passwd = gtk_entry_new();

    gtk_entry_set_visibility(GTK_ENTRY(passwd), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(passwd), '*');

    gtk_fixed_put(GTK_FIXED(loginLayout), username, 85, 220);
    gtk_fixed_put(GTK_FIXED(loginLayout), passwd, 85, 260);

    gtk_widget_show_all(window);

    gtk_main();
    destroy_surfaces();
    return 0;
}