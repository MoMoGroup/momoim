#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <cairo.h>
#include <logger.h>

GtkWidget *background, *headx, *search, *friend,*closebut;
GtkWidget *window;
GtkWidget *frameLayout, *MainLayout;
cairo_surface_t *surface1, *surface2, *surface3, *surface4,*surface51,*surface52,*surface53;
int X = 0;
int Y = 0;

extern CRPPacketInfoData userdata;
extern gchar *uidname;

static void create_surfaces() {

    surface1 = cairo_image_surface_create_from_png("主背景.png");
    surface2 = cairo_image_surface_create_from_png("头像2.png");
    surface3 = cairo_image_surface_create_from_png("搜索.png");
    surface4 = cairo_image_surface_create_from_png("好友.png");
    surface51 = cairo_image_surface_create_from_png("关闭1.png");
    surface52 = cairo_image_surface_create_from_png("关闭2.png");
    surface53 = cairo_image_surface_create_from_png("关闭3.png");

    background = gtk_image_new_from_surface(surface1);
    gtk_fixed_put(GTK_FIXED(MainLayout), background, 0, 0);//起始坐标

    headx = gtk_image_new_from_surface(surface2 );
    gtk_fixed_put(GTK_FIXED(MainLayout), headx, 2, 10);

    search = gtk_image_new_from_surface(surface3);
    gtk_fixed_put(GTK_FIXED(MainLayout), search, 0, 140);

    friend = gtk_image_new_from_surface(surface4);
    gtk_fixed_put(GTK_FIXED(MainLayout), friend, -10, 174);
    closebut = gtk_image_new_from_surface(surface51);
    gtk_fixed_put(GTK_FIXED(MainLayout), closebut, 247, 0);

}

static void
destroy_surfaces() {
    g_print("destroying surfaces2");
    cairo_surface_destroy(surface1);
    cairo_surface_destroy(surface2);
    cairo_surface_destroy(surface3);
    cairo_surface_destroy(surface4);

}
//鼠标点击事件
static gint button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data){
    X = event->x;  // 取得鼠标相对于窗口的位置
    Y = event->y;

    if (event->button == 1 && (X > 247 && X < 280) && (Y> 2 && Y < 25)) {              //设置关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) closebut, surface52); //置换图标
    }
    else{
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
        if (event->button == 1) { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
            gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                event->x_root, event->y_root, event->time);

        }
    }

    return TRUE;
}

//鼠标抬起事件
static gint button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data){

    X = event->x;  // 取得鼠标相对于窗口的位置
    Y = event->y;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *)closebut, surface51);  //设置关闭按钮
      if ((X > 247 && X < 280) && (Y > 2 && Y < 25))
          DeleteEvent();
        destroy_surfaces();
    }

    return TRUE;
}

//鼠标移动事件
static gint motion_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data){

    X = event->x;  // 取得鼠标相对于窗口的位置
    Y = event->y;
    if ((X > 247 && X < 280) && (Y > 2 && Y < 25)){
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
        gtk_image_set_from_surface((GtkImage *) closebut, surface53);
    }
    else {
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
        gtk_image_set_from_surface((GtkImage *) closebut,surface51);
    }

    return TRUE;
}

int maininterface() {

    //gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 284, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    MainLayout = gtk_fixed_new();
    frameLayout = gtk_layout_new(NULL, NULL);

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
    GtkWidget *userid;
    //g_print(userdata.nickName);
    userid = gtk_label_new(userdata.nickName);
    gtk_fixed_put(GTK_FIXED(MainLayout), userid, 170, 90);

    gtk_container_add(GTK_CONTAINER(window), frameLayout);//frameLayout 加入到window
    gtk_container_add(GTK_CONTAINER(frameLayout), MainLayout);

    gtk_widget_show_all(window);
    //gtk_main();

    return 0;
}
