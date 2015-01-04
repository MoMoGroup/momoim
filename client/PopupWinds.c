#include <gtk/gtk.h>
#include <cairo.h>
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
    endface = cairo_image_surface_create_from_png("提示框关闭.png");
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
        gdk_window_set_cursor(gtk_widget_get_window(popupwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
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
            gtk_dialog_response((GtkDialog *) popupwindow, 1);
        }
    }
    return 0;
}

//鼠标移动事件
static gint motion_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    px = event->x;  // 取得鼠标相对于窗口的位置
    py = event->y;
    if ((px > 220 && px < 255) && (py > 2 && py < 28)) {
        gdk_window_set_cursor(gtk_widget_get_window(popupwindow), gdk_cursor_new(GDK_HAND2));
    }
    else if (event->button == 1 && (px > 60 && px < 192) && (py > 178 && py < 208)) {
        gdk_window_set_cursor(gtk_widget_get_window(popupwindow), gdk_cursor_new(GDK_HAND2));

    } else {
        gdk_window_set_cursor(gtk_widget_get_window(popupwindow), gdk_cursor_new(GDK_ARROW));
    }
    return 0;
}

/*static gboolean
on_expose_event(GtkWidget *widget,
        GdkEventExpose *event,
        gpointer data)
{
    cairo_t *cr;
    cr = gdk_cairo_create ((GdkWindow *) widget->priv);
    cairo_paint(cr);
    cairo_destroy(cr);
    return FALSE;
}*/

int popup(const char *title, const char *tell) {
    popupwindow = gtk_dialog_new();

    gtk_window_set_position(GTK_WINDOW(popupwindow), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (popupwindow), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(popupwindow), FALSE);//去掉边框
    //gtk_widget_set_size_request(GTK_WIDGET(popupwindow), 250, 235);

    box = gtk_dialog_get_content_area((GtkDialog *) popupwindow);//得到dialog的box
    action = gtk_dialog_get_action_area((GtkDialog *) popupwindow);//得到dialog的action_area

    //box = gtk_image_new_from_stock(GTK_STOCK_DIALOG_INFO,GTK_ICON_SIZE_DIALOG);
    //box=gtk_image_new_from_file("提示框.png");

    popupLayout = gtk_fixed_new();
    create_popupfaces();

    cairo_t *cr;
    cairo_text_extents_t te;


    GtkWidget *telltitle, *tellyou;
    gtk_container_add(GTK_CONTAINER(box), popupLayout);
    telltitle = gtk_label_new(title);//标题
    tellyou = gtk_label_new(tell);//内容
    PangoFontDescription *font = pango_font_description_new();
    gtk_widget_override_font(tellyou, pango_font_description_from_string("Monospace 15"));
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 100, 100);

    cr = cairo_create(surface);
    cairo_text_extents_t extents;
    cairo_select_font_face(cr, "Monospace",
            CAIRO_FONT_SLANT_NORMAL,
            CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 15);
    cairo_text_extents(cr, tell, &extents);

    gtk_widget_set_size_request(GTK_WIDGET(popupwindow), extents.width + 100, extents.height + 100);
    int x = (extents.width / 2 + extents.x_bearing);
    int y = (extents.height / 2 + extents.y_bearing);

    gtk_fixed_put(GTK_FIXED(popupLayout), telltitle, 14, 10);
    gtk_fixed_put(GTK_FIXED(popupLayout), tellyou, x, 110);

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
    gtk_widget_hide(action);//隐藏留白的action

    gtk_dialog_run(GTK_DIALOG(popupwindow));

    destroy_popfaces();
    gtk_widget_destroy(popupwindow);
    return 0;
}