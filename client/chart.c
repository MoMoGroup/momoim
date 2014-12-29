#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <cairo.h>
#include <logger.h>
#include <imcommon/friends.h>
#include <stdlib.h>

int X = 0;
int Y = 0;
extern int chardestroyflag = 0;

static GtkWidget *window;
static GtkWidget *chartlayout, *chartlayout2;

static cairo_surface_t *surface1, *surface21, *surface22, *surface3, *surface41, *surface42, *surface52, *surface51;
static cairo_surface_t *surface61, *surface62, *surface71, *surface72, *surface73;
static GtkWidget *image1, *image2, *image3, *image4, *image5;
static GtkWidget *image6, *image7;

static void create_surfaces() {


    surface1 = cairo_image_surface_create_from_png("花背景.png");
    surface21 = cairo_image_surface_create_from_png("发送1.png");
    surface22 = cairo_image_surface_create_from_png("发送2.png");
    surface3 = cairo_image_surface_create_from_png("头像3.png");
    surface41 = cairo_image_surface_create_from_png("语音1.png");
    surface42 = cairo_image_surface_create_from_png("语音2.png");
    surface51 = cairo_image_surface_create_from_png("视频1.png");
    surface52 = cairo_image_surface_create_from_png("视频2.png");
    surface61 = cairo_image_surface_create_from_png("关闭1.png");
    surface62 = cairo_image_surface_create_from_png("关闭2.png");
    surface71 = cairo_image_surface_create_from_png("关闭按钮1.png");
    surface72 = cairo_image_surface_create_from_png("关闭按钮2.png");
    surface73 = cairo_image_surface_create_from_png("关闭按钮3.png");

    image1 = gtk_image_new_from_surface(surface1);
    gtk_fixed_put(GTK_FIXED(chartlayout), image1, 0, 0);
//发送
    image2 = gtk_image_new_from_surface(surface21);
    gtk_fixed_put(GTK_FIXED(chartlayout), image2, 390, 510);

//头像
    image3 = gtk_image_new_from_surface(surface3);
    gtk_fixed_put(GTK_FIXED(chartlayout), image3, 0, 0);

//语音
    image4 = gtk_image_new_from_surface(surface41);
    gtk_fixed_put(GTK_FIXED(chartlayout), image4, 15, 70);


//视频按钮
    image5 = gtk_image_new_from_surface(surface51);
    gtk_fixed_put(GTK_FIXED(chartlayout), image5, 60, 70);


//下方关闭按钮
    image6 = gtk_image_new_from_surface(surface61);
    gtk_fixed_put(GTK_FIXED(chartlayout), image6, 300, 512);


//右上角关闭按钮
    image7 = gtk_image_new_from_surface(surface71);
    gtk_fixed_put(GTK_FIXED(chartlayout), image7, 470, 0);

}

static void
destroy_surfaces3() {
    g_print("destroying surfaces3");

    cairo_surface_destroy(surface1);
    cairo_surface_destroy(surface21);
    cairo_surface_destroy(surface22);
    cairo_surface_destroy(surface3);
    cairo_surface_destroy(surface41);
    cairo_surface_destroy(surface42);
    cairo_surface_destroy(surface51);
    cairo_surface_destroy(surface52);
    cairo_surface_destroy(surface61);
    cairo_surface_destroy(surface62);
    cairo_surface_destroy(surface71);
    cairo_surface_destroy(surface72);
    cairo_surface_destroy(surface73);
}

//鼠标点击事件
static gint button_press_event(GtkWidget *widget,

GdkEventButton *event, gpointer data) {
    X = event->x;  // 取得鼠标相对于窗口的位置
    Y = event->y;

if (event->button == 1 && (X > 391 && X < 473) && (Y > 513 && Y < 540) && chardestroyflag == 0) {     //设置发送按钮
        g_print("hello");
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
gtk_image_set_from_surface((GtkImage *) image2, surface22); //置换图标
    }
    else if (event->button == 1 && (X > 18 && X < 43) && (Y > 70 && Y < 100) && chardestroyflag == 0) {   //设置语音按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
gtk_image_set_from_surface((GtkImage *) image4, surface42); //置换图标
    }
    else if (event->button == 1 && (X > 60 && X < 93) && (Y > 74 && Y < 103) && chardestroyflag == 0) {   //设置视频按钮
        g_print("hello");
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
gtk_image_set_from_surface((GtkImage *) image5, surface52); //置换图标
    }
    else if (event->button == 1 && (X > 301 && X < 382) && (Y > 513 && Y < 540) && chardestroyflag == 0) {          //设置右下关闭按钮
        g_print("hello");
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) image6, surface62); //置换图标
    }
    else if (event->button == 1 && (X > 470 && X < 500) && (Y > 2 && Y < 25) && chardestroyflag == 0) {         //设置右上关闭按钮
        g_print("hello");
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) image7, surface72); //置换图标
    }
    else {
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
        if (event->button == 1) { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
            gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                    event->x_root, event->y_root, event->time);
        }
    }

    return 0;
}

//鼠标抬起事件
static gint button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data) {

    X = event->x;  // 取得鼠标相对于窗口的位置
    Y = event->y;
    if (event->button == 1 && chardestroyflag == 0)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) image2, surface21);
        gtk_image_set_from_surface((GtkImage *) image4, surface41);
        gtk_image_set_from_surface((GtkImage *) image5, surface51);
        gtk_image_set_from_surface((GtkImage *) image6, surface61);//设置右下关闭
        gtk_image_set_from_surface((GtkImage *) image7, surface71);  //设置右上关闭按钮
        if (((X > 470 && X < 500) && (Y > 2 && Y < 25)) || ((X > 301 && X < 382) && (Y > 513 && Y < 540))) {
            destroy_surfaces3();
            //DeleteEvent();
            gtk_widget_destroy(window);
    chardestroyflag == 1;
        }
        return 0;
    }
}

//鼠标移动事件
static gint motion_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data) {

    X = event->x;  // 取得鼠标相对于窗口的位置
    Y = event->y;
        if ((X > 391 && X < 473) && (Y > 513 && Y < 540) && chardestroyflag == 0) {     //设置发送按钮

        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) image2, surface22); //置换图标
    }
    else if ((X > 18 && X < 43) && (Y > 70 && Y < 100) && chardestroyflag == 0) {   //设置语音按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) image4, surface42); //置换图标
    }
    else if ((X > 60 && X < 93) && (Y > 74 && Y < 103) && chardestroyflag == 0) {   //设置视频按钮

        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) image5, surface52); //置换图标
    }
    else if ((X > 301 && X < 382) && (Y > 513 && Y < 540) && chardestroyflag == 0) {          //设置右下关闭按钮

        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
gtk_image_set_from_surface((GtkImage *) image6, surface62); //置换图标
    }
    else if ((X > 470 && X < 500) && (Y > 2 && Y < 25) && chardestroyflag == 0) {         //设置右上关闭按钮

        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
gtk_image_set_from_surface((GtkImage *) image7, surface73); //置换图标
    }
    else {
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
        gtk_image_set_from_surface((GtkImage *) image2, surface21);
        gtk_image_set_from_surface((GtkImage *) image4, surface41);
        gtk_image_set_from_surface((GtkImage *) image5, surface51);
        gtk_image_set_from_surface((GtkImage *) image6, surface61);//设置右下关闭
        gtk_image_set_from_surface((GtkImage *) image7, surface71);  //设置右上关闭按钮
    }

    return 0;
}

int mainchart() {


    //创建窗口，并为窗口的关闭信号加回调函数以便退出
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    chartlayout = gtk_fixed_new();
    chartlayout2 = gtk_layout_new(NULL, NULL);

    gtk_container_add(GTK_CONTAINER (window), chartlayout2);//chartlayout2 加入到window
    gtk_container_add(GTK_CONTAINER (chartlayout2), chartlayout);

    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(gtk_main_quit), NULL);

    gtk_window_set_default_size(GTK_WINDOW(window), 500, 550);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);//窗口出现位置
    //gtk_window_set_resizable (GTK_WINDOW (window), FALSE);//窗口不可改变

    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);   // 去掉边框
// 设置窗体获取鼠标事件
    gtk_widget_set_events(window,

            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK

                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK

                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

    g_signal_connect(G_OBJECT(window), "button_press_event",
            G_CALLBACK(button_press_event), window);       // 加入事件回调
    g_signal_connect(G_OBJECT(window), "motion_notify_event",
            G_CALLBACK(motion_notify_event), window);

    g_signal_connect(G_OBJECT(window), "button_release_event",
            G_CALLBACK(button_release_event), window);

    create_surfaces();
    GtkWidget *text1, *text2;
    //创建发送文本框，和接受文本狂
    text1 = gtk_text_view_new();
    text2 = gtk_text_view_new();

    GtkScrolledWindow *sw1 = gtk_scrolled_window_new(NULL, NULL);
    GtkScrolledWindow *sw2 = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(sw1), text1);

    gtk_container_add(GTK_CONTAINER(sw2), text2);

    gtk_text_view_set_wrap_mode(text1, GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_wrap_mode(text2, GTK_WRAP_WORD_CHAR);//自动换行

    gtk_text_view_set_editable(text2,
            0);//不可编辑

    gtk_fixed_put(GTK_FIXED(chartlayout), sw1, 0, 425);//文本框位置
    gtk_fixed_put(GTK_FIXED(chartlayout), sw2, 0, 115);

    gtk_widget_set_size_request(sw1, 500, 80);
    gtk_widget_set_size_request(sw2, 500, 300);//大小

    GdkRGBA rgba = {
    1, 1, 1, 0
    };
    gtk_widget_override_background_color(text1, GTK_STATE_NORMAL, &rgba);//设置透明
    gtk_widget_override_background_color(text2, GTK_STATE_NORMAL, &rgba);//设置透明

    gtk_widget_show_all(window);

    // gtk_main();

    return 0;
}