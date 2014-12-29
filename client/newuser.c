#include <gtk/gtk.h>
#include "newuser.h"

GtkWidget *newwindow;
GtkWidget *zhuceLayout;
GtkWidget *mnickname, *username, *passwd1, *passwd2;
GtkWidget *background, *headline, *nickid, *nick, *nickmm1, *nickmm2, *mminfo, *endwind;
cairo_surface_t *sflowerbackgroud, *surface2, *surfaceresearch, *surfacefriendimage, *surface5, *surface6, *surface7, *surface8, *surface82, *surface83;
int mx = 0;
int my = 0;

static void create_zhucefaces() {

    sflowerbackgroud = cairo_image_surface_create_from_png("注册背景.png");
    surface2 = cairo_image_surface_create_from_png("注册标题.png");
    surfaceresearch = cairo_image_surface_create_from_png("注册按钮.png");
    surfacefriendimage = cairo_image_surface_create_from_png("资料1.png");
    surface5 = cairo_image_surface_create_from_png("资料2.png");
    surface6 = cairo_image_surface_create_from_png("资料3.png");
    surface7 = cairo_image_surface_create_from_png("资料4.png");

    surface8 = cairo_image_surface_create_from_png("关闭1.png");
    surface82 = cairo_image_surface_create_from_png("关闭2.png");
    surface83 = cairo_image_surface_create_from_png("关闭3.png");

    background = gtk_image_new_from_surface(sflowerbackgroud);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), background, 0, 0);//起始坐标

    headline = gtk_image_new_from_surface(surface2);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), headline, 2, 10);

    mminfo = gtk_image_new_from_surface(surfaceresearch);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), mminfo, 255, 400);

    nickid = gtk_image_new_from_surface(surfacefriendimage);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), nickid, -2, 80);

    nick = gtk_image_new_from_surface(surface5);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), nick, 10, 150);

    nickmm1 = gtk_image_new_from_surface(surface6);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), nickmm1, -2, 215);

    nickmm2 = gtk_image_new_from_surface(surface7);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), nickmm2, -2, 290);

    endwind = gtk_image_new_from_surface(surface8);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), endwind, 375, 0);

}

static void
destroy_surfaces() {
    g_print("destroying surfaces2");
    cairo_surface_destroy(sflowerbackgroud);
    cairo_surface_destroy(surface2);
    cairo_surface_destroy(surfaceresearch);
    cairo_surface_destroy(surfacefriendimage);
    cairo_surface_destroy(surface5);
    cairo_surface_destroy(surface6);
    cairo_surface_destroy(surface7);
}

//鼠标点击事件
static gint button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    mx = event->x;  // 取得鼠标相对于窗口的位置
    my = event->y;

    if (event->button == 1 && (mx > 375 && mx < 414) && (my > 0 && my < 24)) {              //设置关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) endwind, surface82); //置换图标
    }
    else if (event->button == 1 && (mx > 260 && mx < 400) && (my > 405 && my < 450)) {

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

    if (event->button == 1)       // 判断是否是点击关闭图标
    {
        gtk_image_set_from_surface((GtkImage *) endwind, surface83);  //设置关闭按钮
        if ((mx > 375 && mx < 414) && (my > 0 && my < 24)) {
            destroy_surfaces();
            //gtk_main_quit();
        }
    }

    return 0;
}

//鼠标移动事件
static gint motion_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    mx = event->x;  // 取得鼠标相对于窗口的位置
    my = event->y;
    if ((mx > 375 && mx < 414) && (my > 0 && my < 24)) {
        gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_HAND2));
        gtk_image_set_from_surface((GtkImage *) endwind, surface83);
    }
    else {
        gdk_window_set_cursor(gtk_widget_get_window(newwindow), gdk_cursor_new(GDK_ARROW));
        gtk_image_set_from_surface((GtkImage *) endwind, surface8);
    }

    return 0;
}

int newface() {

    newwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(newwindow), 414, 471);
    gtk_window_set_position(GTK_WINDOW(newwindow), GTK_WIN_POS_MOUSE);
    gtk_window_set_decorated(GTK_WINDOW(newwindow), FALSE);

    zhuceLayout = gtk_fixed_new();
    create_zhucefaces();

    gtk_container_add(GTK_CONTAINER(newwindow), zhuceLayout);

    mnickname = gtk_entry_new();
    username = gtk_entry_new();
    passwd1 = gtk_entry_new();
    passwd2 = gtk_entry_new();

    gtk_entry_set_visibility(GTK_ENTRY(passwd1), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(passwd1), '*');
    gtk_entry_set_visibility(GTK_ENTRY(passwd2), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(passwd2), '*');

    gtk_fixed_put(GTK_FIXED(zhuceLayout), mnickname, 85, 120);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), username, 85, 180);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), passwd1, 85, 255);
    gtk_fixed_put(GTK_FIXED(zhuceLayout), passwd2, 85, 326);

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