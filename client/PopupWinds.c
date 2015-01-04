#include <gtk/gtk.h>
#include <cairo.h>
#include <string.h>
#include "PopupWinds.h"

GtkWidget *popupwindow; //dialog
GtkWidget *popupLayout; //放入box
GtkWidget *popback, *popend, *popanniu;
GtkWidget *box, *action; //dialog的box和action_area
cairo_surface_t *backface, *endface, *anniuface;
int px = 0;
int py = 0;

static void create_popupfaces() {
    backface = cairo_image_surface_create_from_png("提示框.png");
    //endface = cairo_image_surface_create_from_png("提示框关闭.png");
    anniuface = cairo_image_surface_create_from_png("提示框按钮1.png");

    popback = gtk_image_new_from_surface(backface);
    gtk_fixed_put(GTK_FIXED(popupLayout), popback, 0, 0);//起始坐标

    popend = gtk_image_new_from_surface(endface);
    gtk_fixed_put(GTK_FIXED(popupLayout), popend, 220, 2);

    popanniu = gtk_image_new_from_surface(anniuface);
    gtk_fixed_put(GTK_FIXED(popupLayout), popanniu, 52, 170);

}

static void
destroy_popfaces() {
    g_print("destroying poppuwinds");
    cairo_surface_destroy(backface);
    cairo_surface_destroy(endface);
    cairo_surface_destroy(anniuface);
}

//鼠标点击事件
static gint button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    px = event->x;  // 取得鼠标相对于窗口的位置
    py = event->y;

    if (event->button == 1 && (px > 220 && px < 255) && (py > 2 && py < 28)) {              //设置关闭按钮
        //gdk_window_set_cursor(gtk_widget_get_window(popupwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    else if (event->button == 1 && (px > 60 && px < 192) && (py > 178 && py < 208)) {        //设置注册按钮
        gdk_window_set_cursor(gtk_widget_get_window(popupwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标

    }
    else {
        gdk_window_set_cursor(gtk_widget_get_window(popupwindow), gdk_cursor_new(GDK_ARROW));
        if (event->button == 1) { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
            gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                    event->x_root, event->y_root, event->time);
        }
    }
    return 0;
}

//鼠标抬起事件
static gint button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    px = event->x;  // 取得鼠标相对于窗口的位置
    py = event->y;

    if (event->button == 1 && (px > 60 && px < 192) && (py > 178 && py < 208)) {
        gtk_dialog_response((GtkDialog *) popupwindow, 1);
    }
    else if (event->button == 1)       // 判断是否是点击关闭图标
    {
        if ((px > 220 && px < 250) && (py > 2 && py < 28)) {
            //gtk_dialog_response((GtkDialog *) popupwindow, 1);
        }
    }
    return 0;
}

//鼠标移动事件
static gint motion_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    px = event->x;  // 取得鼠标相对于窗口的位置
    py = event->y;
    if ((px > 220 && px < 255) && (py > 2 && py < 28)) {
        //gdk_window_set_cursor(gtk_widget_get_window(popupwindow), gdk_cursor_new(GDK_HAND2));
    }
    else if (event->button == 1 && (px > 60 && px < 192) && (py > 178 && py < 208)) {
        gdk_window_set_cursor(gtk_widget_get_window(popupwindow), gdk_cursor_new(GDK_HAND2));

    } else {
        gdk_window_set_cursor(gtk_widget_get_window(popupwindow), gdk_cursor_new(GDK_ARROW));
    }
    return 0;
}

int popup(const char *title, const char *tell) {
    popupwindow = gtk_dialog_new();
    gtk_window_set_position(GTK_WINDOW(popupwindow), GTK_WIN_POS_CENTER);//窗口位置
    //gtk_window_set_resizable(GTK_WINDOW (popupwindow), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(popupwindow), FALSE);//去掉边框
    //gtk_widget_set_size_request(GTK_WIDGET(popupwindow), 250, 235);

    box = gtk_dialog_get_content_area((GtkDialog *) popupwindow);//得到dialog的box
    action = gtk_dialog_get_action_area((GtkDialog *) popupwindow);//得到dialog的action_area
    int maWidth, maHeight;
    popupLayout = gtk_fixed_new();
    create_popupfaces();

    cairo_t *cr;
    cairo_text_extents_t te;


    GtkWidget *telltitle, *tellyou;
    gtk_container_add(GTK_CONTAINER(box), popupLayout);
    telltitle = gtk_label_new(title);//标题
    tellyou = gtk_label_new(tell);//内容

    PangoContext *context = gtk_widget_get_pango_context(tellyou);

    PangoLayout *layout = pango_layout_new(context);
    //pango_layout_set_font_description(layout, pango_font_description_from_string("Monospace 15"));
    pango_layout_set_text(layout, tell, strlen(tell));
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_get_pixel_size(layout, &maWidth, &maHeight);
    if (maWidth > 400) maWidth = 400;
    //gtk_label_set_justify(GTK_LABEL(tellyou),GTK_JUSTIFY_CENTER);/*设置标号对齐方式为居中对齐*/
    //gtk_label_set_line_wrap(GTK_LABEL(tellyou),TRUE);/*打开自动换行*/

    gtk_fixed_put(GTK_FIXED(popupLayout), telltitle, 14, 10);
    if (80 + maWidth < 250) {
        gtk_fixed_put(GTK_FIXED(popupLayout), tellyou, (250 - maWidth) / 2, 110);
        gtk_widget_set_size_request(GTK_WIDGET(popupwindow), 250, 235);
    } else {
        gtk_fixed_put(GTK_FIXED(popupLayout), tellyou, 40, 110);
        gtk_widget_set_size_request(GTK_WIDGET(popupwindow), 80 + maWidth, 235);
    }


    // 设置窗体获取鼠标事件
    gtk_widget_set_events(popupwindow,

            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK

                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK

                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

    g_signal_connect(G_OBJECT(popupwindow), "button_press_event",
            G_CALLBACK(button_press_event), popupwindow);    //加入事件回调

    g_signal_connect(G_OBJECT(popupwindow), "motion_notify_event",
            G_CALLBACK(motion_notify_event), popupwindow);

    g_signal_connect(G_OBJECT(popupwindow), "button_release_event",
            G_CALLBACK(button_release_event), popupwindow);

    gtk_widget_show_all(popupwindow);
    //gtk_widget_hide(action);//隐藏留白的action

    gtk_dialog_run(GTK_DIALOG(popupwindow));

    destroy_popfaces();
    gtk_widget_destroy(popupwindow);
    return 0;
}