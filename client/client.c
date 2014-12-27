#include <gtk/gtk.h>
#include <stdlib.h>
#include "ClientSockfd.h"
#include "MainInterface.h"

GtkWidget *username, *passwd;
int nX = 0;
int nY = 0;
GtkWidget *window;
GtkWidget *image4, *image8, *image7;
int flag = 1;
GtkWidget *loginLayout, *pendingLayout, *frameLayout;

//关闭窗口
int DeleteEvent() {
    gtk_main_quit();
    return TRUE;
}

void *sendhello(void *M) {
    mysockfd();
    return 0;
}

void on_button_clicked() {

    pthread_t mythread;

    gtk_widget_hide(loginLayout);//隐藏loginlayout
    flag = 0;
    //gtk_widget_destroy(layout);销毁layout对话框

    GtkWidget *image1, *image2, *image3;
    pendingLayout = gtk_fixed_new();

    gtk_container_add(GTK_CONTAINER (frameLayout), pendingLayout);
    image1 = gtk_image_new_from_file("背景.png");
    gtk_fixed_put(GTK_FIXED(pendingLayout), image1, 0, 0);//起始坐标

    image2 = gtk_image_new_from_file("狗狗.png");
    gtk_fixed_put(GTK_FIXED(pendingLayout), image2, 45, 150);

    image3 = gtk_image_new_from_file("玩命登陆.png");
    gtk_fixed_put(GTK_FIXED(pendingLayout), image3, 40, 60);


    gtk_widget_show_all(pendingLayout);//显示layout2

    pthread_create(&mythread, NULL, sendhello, NULL);


}

static gint button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data) {
    nX = event->x;  // 取得鼠标相对于窗口的位置
    nY = event->y;
    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        if (event->button == 1 && (nX > 75 && nX < 205) && (nY > 302 && nY < 335) && flag == 1)       // 判断是否左键按下

        {
            gtk_image_set_from_file((GtkImage *) image4, "登陆按钮2.png");// 判断是否在关闭图标区域中

        }
        else if (event->button == 1 && (nX > 260 && nX < 280) && (nY > 2 && nY < 25) && flag == 1)
        {
            gtk_image_set_from_file((GtkImage *) image8, "关闭2.png"); //置换图标

        }
        else if (event->button == 1 && (nX > 5 && nX < 62) && (nY > 380 && nY < 395) && flag == 1)       // 判断是否左键按下

        {
            gtk_image_set_from_file((GtkImage *) image7, "注册账号2.png");
        }
        else
        {
            if (event->button == 1)
            { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
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
    if (event->button == 1 & (nX > 75 && nX < 205) && (nY > 302 && nY < 335) & flag == 1)  //判断是否在登陆区域中
    {
        gtk_image_set_from_file((GtkImage *) image4, "登陆按钮.png");
        on_button_clicked();
    }
    else if (event->button == 1&&flag == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_file((GtkImage *) image8, "关闭.png");
        if((nX > 260 && nX < 280) && (nY > 2 && nY < 25))
        DeleteEvent();
    }

    return TRUE;
}

static gint motion_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 鼠标移动事件

{
    nX = event->x;  // 取得鼠标相对于窗口的位置
    nY = event->y;
    if (flag == 1)
    {
        if ((nX > 75 && nX < 205) && (nY > 302 && nY < 335))
        {
            gtk_image_set_from_file((GtkImage *) image4, "登陆按钮3.png");
        }
        else
        {
            gtk_image_set_from_file((GtkImage *) image4, "登陆按钮.png");
        }
    }

    return TRUE;
}

int main(int argc, char *argv[])
{


    GtkWidget *image1, *image2, *image3, *image5, *image6;

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


    frameLayout = gtk_layout_new(NULL, NULL);
    loginLayout = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER (window), frameLayout);//frameLayout 加入到window
    gtk_container_add(GTK_CONTAINER (frameLayout), loginLayout);

    image1 = gtk_image_new_from_file("背景.png");
    gtk_fixed_put(GTK_FIXED(loginLayout), image1, 0, 0);//起始坐标

    image2 = gtk_image_new_from_file("头像.png");
    gtk_fixed_put(GTK_FIXED(loginLayout), image2, 65, 30);

    image3 = gtk_image_new_from_file("白色.png");
    gtk_fixed_put(GTK_FIXED(loginLayout), image3, 25, 200);

    image4 = gtk_image_new_from_file("登陆按钮.png");
    gtk_fixed_put(GTK_FIXED(loginLayout), image4, 70, 300);

    image5 = gtk_image_new_from_file("账号.png");
    gtk_fixed_put(GTK_FIXED(loginLayout), image5, 35, 220);

    image6 = gtk_image_new_from_file("密码.png");
    gtk_fixed_put(GTK_FIXED(loginLayout), image6, 35, 260);

//    GtkWidget *username, *passwd;
    username = gtk_entry_new();
    passwd = gtk_entry_new();

    gtk_entry_set_visibility(GTK_ENTRY(passwd), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(passwd), '*');

    gtk_fixed_put(GTK_FIXED(loginLayout), username, 85, 220);
    gtk_fixed_put(GTK_FIXED(loginLayout), passwd, 85, 260);

    image7 = gtk_image_new_from_file("注册账号.png");
    gtk_fixed_put(GTK_FIXED(loginLayout), image7, 5, 380);

    image8 = gtk_image_new_from_file("关闭.png");
    gtk_fixed_put(GTK_FIXED(loginLayout), image8, 260, 0);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}