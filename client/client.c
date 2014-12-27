#include <gtk/gtk.h>
#include <stdlib.h>
#include "ClientSockfd.h"
#include "MainInterface.h"
#include <cairo.h>
#include <bits/sigthread.h>
#include <sys/socket.h>
#include <logger.h>

GtkWidget *image4, *image7, *image8, *image10;
GtkWidget *username, *passwd;
pthread_t thread1;
int sockfd;
int nX = 0;
int nY = 0;
GtkWidget *window;

cairo_surface_t *surface1, *surface2, *surface3, *surface41, *surface42, *surface43, *surface5, *surface6;
cairo_surface_t *surface71, *surface72, *surface81, *surface82, *surface83, *surface9, *surface10_1, *surface10_2, *surface10_3;
int flag = 1;
GtkWidget *loginLayout, *pendingLayout, *frameLayout;

//关闭窗口
static void
create_surfaces1() {
    GtkWidget *image1, *image2, *image3, *image5, *image6;
    surface1 = cairo_image_surface_create_from_png("背景.png");
    surface2 = cairo_image_surface_create_from_png("头像.png");
    surface3 = cairo_image_surface_create_from_png("白色.png");
    surface41 = cairo_image_surface_create_from_png("登陆按钮.png");
    surface42 = cairo_image_surface_create_from_png("登陆按钮2.png");
    surface43 = cairo_image_surface_create_from_png("登陆按钮3.png");
    surface5 = cairo_image_surface_create_from_png("账号.png");
    surface6 = cairo_image_surface_create_from_png("密码.png");
    surface71 = cairo_image_surface_create_from_png("注册账号.png");
    surface72 = cairo_image_surface_create_from_png("注册账号2.png");
    surface81 = cairo_image_surface_create_from_png("关闭1.png");
    surface82 = cairo_image_surface_create_from_png("关闭2.png");
    surface83 = cairo_image_surface_create_from_png("关闭3.png");


    image1 = gtk_image_new_from_surface(surface1);
    gtk_fixed_put(GTK_FIXED(loginLayout), image1, 0, 0);//起始坐标

    image2 = gtk_image_new_from_surface(surface2);
    gtk_fixed_put(GTK_FIXED(loginLayout), image2, 65, 30);

    image3 = gtk_image_new_from_surface(surface3);
    gtk_fixed_put(GTK_FIXED(loginLayout), image3, 25, 200);

    image4 = gtk_image_new_from_surface(surface41);
    gtk_fixed_put(GTK_FIXED(loginLayout), image4, 70, 300);

    image5 = gtk_image_new_from_surface(surface5);
    gtk_fixed_put(GTK_FIXED(loginLayout), image5, 35, 220);

    image6 = gtk_image_new_from_surface(surface6);
    gtk_fixed_put(GTK_FIXED(loginLayout), image6, 35, 260);
    image7 = gtk_image_new_from_surface(surface71);
    gtk_fixed_put(GTK_FIXED(loginLayout), image7, 5, 380);

    image8 = gtk_image_new_from_surface(surface81);
    gtk_fixed_put(GTK_FIXED(loginLayout), image8, 247, 0);
}


static void create_surfaces2() {
    GtkWidget *image1, *image2;
    surface9 = cairo_image_surface_create_from_png("玩命登陆.png");
    surface10_1 = cairo_image_surface_create_from_png("取消1.png");
    surface10_2 = cairo_image_surface_create_from_png("取消2.png");
    surface10_3 = cairo_image_surface_create_from_png("取消3.png");

    image1 = gtk_image_new_from_file("等待.gif");
    gtk_fixed_put(GTK_FIXED(pendingLayout), image1, 0, 0);

    image2 = gtk_image_new_from_surface(surface9);
    gtk_fixed_put(GTK_FIXED(pendingLayout), image2, 40, 50);

    image10 = gtk_image_new_from_surface(surface10_1);
    gtk_fixed_put(GTK_FIXED(pendingLayout), image10, 70, 310);

}

static void
destroy_surfaces() {
    g_print("destroying surfaces");
    cairo_surface_destroy(surface1);
    cairo_surface_destroy(surface2);
    cairo_surface_destroy(surface3);
    cairo_surface_destroy(surface41);
    cairo_surface_destroy(surface42);
    cairo_surface_destroy(surface43);
    cairo_surface_destroy(surface5);
    cairo_surface_destroy(surface6);
    cairo_surface_destroy(surface71);
    cairo_surface_destroy(surface72);
    cairo_surface_destroy(surface81);
    cairo_surface_destroy(surface82);
    cairo_surface_destroy(surface83);
    cairo_surface_destroy(surface9);
    cairo_surface_destroy(surface10_1);
    cairo_surface_destroy(surface10_2);
    cairo_surface_destroy(surface10_3);
}

int DeleteEvent() {
    gtk_main_quit();
    return TRUE;
}

void *sendhello(void *M) {
    mysockfd();
    return 0;
}

void on_button_clicked() {


    gtk_widget_hide(loginLayout);//隐藏loginlayout
    flag = 0;
    //gtk_widget_destroy(layout);销毁layout对话框


    pendingLayout = gtk_fixed_new();

    gtk_container_add(GTK_CONTAINER (frameLayout), pendingLayout);

    create_surfaces2();
    gtk_widget_show_all(pendingLayout);//显示layout2

    pthread_create(&thread1, NULL, sendhello, NULL);


}

static gint button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data) {
    nX = event->x;  // 取得鼠标相对于窗口的位置
    nY = event->y;
    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        if (event->button == 1 && (nX > 75 && nX < 205) && (nY > 302 && nY < 335) && flag == 1)    // 判断是否左键按下且在区域内,设置登陆按钮

        {
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
            gtk_image_set_from_surface((GtkImage *) image4, surface42);
        }
        else if (event->button == 1 && (nX > 247 && nX < 280) && (nY > 2 && nY < 25) && flag == 1) {              //设置关闭按钮
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
            gtk_image_set_from_surface((GtkImage *) image8, surface82); //置换图标
        }
        else if (event->button == 1 && (nX > 5 && nX < 62) && (nY > 380 && nY < 395) && flag == 1)       // 判断是否左键按下

        {                                                                                           //设置注册按钮
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
            gtk_image_set_from_surface((GtkImage *) image7, surface72);
        }
        else if (event->button == 1 && (nX > 75 && nX < 202) && (nY > 312 && nY < 350) && flag == 0) {   //设置第二界面取消按钮
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
            gtk_image_set_from_surface((GtkImage *) image10, surface10_3);//设置鼠标光标
        }
        else {                                                                               //设置在非按钮区域内移动窗口
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
            if (event->button == 1) { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
                gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                        event->x_root, event->y_root, event->time);
            }
        }
    }
    return TRUE;

}

static gint button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 鼠标抬起事件

{
    nX = event->x;  // 取得鼠标相对于窗口的位置
    nY = event->y;
    if (event->button == 1 && (nX > 75 && nX < 205) && (nY > 302 && nY < 335) && flag == 1)  //判断是否在登陆区域中，设置登陆按钮
    {
        gtk_image_set_from_surface((GtkImage *) image4, surface41);
        on_button_clicked();
    }
    else if (event->button == 1 && flag == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) image8, surface81);  //设置关闭按钮
        if ((nX > 247 && nX < 280) && (nY > 2 && nY < 25))
            DeleteEvent();
    }
    else if (flag == 0) {                                         //设置取消按钮
        if (event->button == 1 && (nX > 75 && nX < 202) && (nY > 312 && nY < 355)) {
            gtk_image_set_from_surface((GtkImage *) image10, surface10_1);
            if ((nX > 75 && nX < 202) && (nY > 312 && nY < 355)) {
                g_print("hello");
                close(sockfd);
                pthread_cancel(thread1);
                flag = 1;
                gtk_widget_destroy(pendingLayout);
                gtk_widget_show_all(loginLayout);
            }
        }
    }

    return TRUE;
}

static gint motion_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 鼠标移动事件

{
    nX = event->x;  // 取得鼠标相对于窗口的位置
    nY = event->y;
    if (flag == 1) {
        if ((nX > 75 && nX < 205) && (nY > 302 && nY < 335)) {
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
            gtk_image_set_from_surface((GtkImage *) image4, surface43);
        }
        else if ((nX > 247 && nX < 280) && (nY > 2 && nY < 25)) {
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
            gtk_image_set_from_surface((GtkImage *) image8, surface83);
        }
        else if ((nX > 5 && nX < 62) && (nY > 380 && nY < 395)) {
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
        }
        else {
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));

            gtk_image_set_from_surface((GtkImage *) image8, surface81);
            gtk_image_set_from_surface((GtkImage *) image4, surface41);
        }
    }
    else if (flag == 0) {
        if ((nX > 75 && nX < 202) && (nY > 312 && nY < 355)) {
            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
            gtk_image_set_from_surface((GtkImage *) image10, surface10_2);
        }
        else {

            gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
            gtk_image_set_from_surface((GtkImage *) image10, surface10_1);

        }
    }

    return TRUE;
}

int main(int argc, char *argv[]) {

    //初始化GTK+程序
    gtk_init(&argc, &argv);
    //创建窗口，并为窗口的关闭信号加回调函数以便退出
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 283, 411);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);//窗口出现位置
    // gtk_window_set_resizable (GTK_WINDOW (window), FALSE);//窗口不可改变

    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);   // 去掉边框

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