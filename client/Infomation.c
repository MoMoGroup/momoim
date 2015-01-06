#include <gtk/gtk.h>
#include <stdlib.h>


static GtkWidget *Infowind;
static GtkWidget *Infolayout;
static GtkWidget *Infobackg_event_box, *Save_event_box, *Cancel_event_box, *Guanxx_event_box;
static GtkWidget *Infobackground, *Infosave, *Infocancel, *Infoguanbi;
static cairo_surface_t *Surfaceback, *Surfacesave, *Surfacesave1, *Surfacecancel, *Surfacecancel1, *Surfaceend, *Surfaceend1, *Surfaceend2;

static void create_infofaces() {

    Surfaceback = cairo_image_surface_create_from_png("资料.png");
    Surfacesave = cairo_image_surface_create_from_png("保存.png");
    Surfacesave1 = cairo_image_surface_create_from_png("保存2.png");
    Surfacecancel = cairo_image_surface_create_from_png("取消.png");
    Surfacecancel1 = cairo_image_surface_create_from_png("取消2.png");
    Surfaceend = cairo_image_surface_create_from_png("关闭按钮1.png");
    Surfaceend1 = cairo_image_surface_create_from_png("关闭按钮2.png");
    Surfaceend2 = cairo_image_surface_create_from_png("关闭按钮3.png");

    Infobackground = gtk_image_new_from_surface(Surfaceback);
    gtk_container_add(GTK_CONTAINER(Infobackg_event_box), Infobackground);
    gtk_fixed_put(GTK_FIXED(Infolayout), Infobackg_event_box, 0, 0);//起始坐标
    gtk_widget_set_size_request(GTK_WIDGET(Infobackground), 550, 488);

    Infosave = gtk_image_new_from_surface(Surfacesave);
    gtk_container_add(GTK_CONTAINER(Save_event_box), Infosave);
    gtk_fixed_put(GTK_FIXED(Infolayout), Save_event_box, 350, 440);

    Infocancel = gtk_image_new_from_surface(Surfacecancel);
    gtk_container_add(GTK_CONTAINER(Cancel_event_box), Infocancel);
    gtk_fixed_put(GTK_FIXED(Infolayout), Cancel_event_box, 450, 440);

    Infoguanbi = gtk_image_new_from_surface(Surfaceend);
    gtk_container_add(GTK_CONTAINER(Guanxx_event_box), Infoguanbi);
    gtk_fixed_put(GTK_FIXED(Infolayout), Guanxx_event_box, 509, 0);

}

static void destroy_infosurfaces() {
    g_print("destroying infoface");
    cairo_surface_destroy(Surfaceback);
    cairo_surface_destroy(Surfacesave);
    cairo_surface_destroy(Surfacesave1);
    cairo_surface_destroy(Surfacecancel);
    cairo_surface_destroy(Surfacecancel1);
    cairo_surface_destroy(Surfaceend);
    cairo_surface_destroy(Surfaceend1);
    cairo_surface_destroy(Surfaceend2);
}

//背景的eventbox拖曳窗口
static gint Infobackg_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1) { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                event->x_root, event->y_root, event->time);
    }
    return 0;
}

//保存
//鼠标点击事件
static gint save_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {        //设置注册按钮
        gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) Infosave, Surfacesave1); //置换图标
    }
    return 0;
}

//保存
//鼠标抬起事件
static gint save_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        //newsockfd();
    }

    return 0;
}

//保存
//鼠标移动事件
static gint save_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));
    //gtk_image_set_from_surface((GtkImage *) mminfo, surface33);
    return 0;
}

//保存
//鼠标离开事件
static gint save_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) Infosave, Surfacesave);

    return 0;
}

//取消
//鼠标点击事件
static gint cancel_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {        //设置注册按钮
        gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) Infocancel, Surfacecancel1); //置换图标
    }
    return 0;
}

//取消
//鼠标抬起事件
static gint cancel_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        destroy_infosurfaces();
        gtk_widget_destroy(Infowind);
    }

    return 0;
}

//取消
//鼠标移动事件
static gint cancel_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));
    //gtk_image_set_from_surface((GtkImage *) Infocancel, surface33);
    return 0;
}

//取消
//鼠标离开事件
static gint cancel_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) Infocancel, Surfacecancel);

    return 0;
}

//关闭
//鼠标点击事件
static gint guanxx_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {        //设置注册按钮
        gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) Infoguanbi, Surfaceend1); //置换图标
    }
    return 0;
}

//关闭
//鼠标抬起事件
static gint guanxx_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    if (event->button == 1) {
        destroy_infosurfaces();
        gtk_widget_destroy(Infowind);
    }
    return 0;
}

//关闭
//鼠标移动事件
static gint guanxx_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) Infoguanbi, Surfaceend2);
    return 0;
}

//关闭
//鼠标离开事件
static gint guanxx_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    gdk_window_set_cursor(gtk_widget_get_window(Infowind), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) Infoguanbi, Surfaceend);

    return 0;
}

/*// 使用GtkTreeModel作为下拉列表框的数据来源
static GtkWidget *create_combobox_with_model()
{
    GtkWidget *combobox = NULL;
    GtkListStore *store = NULL;
    GtkTreeIter iter;
    GtkCellRenderer *renderer = NULL;

    // 填充数据。在这里只需要一列字符串。
    store = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "home", -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "work", -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "public", -1);

    combobox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
    // 设置GtkTreeModel中的每一项数据如何在列表框中显示
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combobox), renderer, TRUE);
    gtk_cell_layout_set_attributes(
            GTK_CELL_LAYOUT(combobox), renderer, "text", 0, NULL);
    return combobox;
}*/

int info() {

    Infowind = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(Infowind), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (Infowind), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(Infowind), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(Infowind), 550, 488);

    Infobackg_event_box = gtk_event_box_new();
    Save_event_box = gtk_event_box_new();
    Cancel_event_box = gtk_event_box_new();
    Guanxx_event_box = gtk_event_box_new();

    Infolayout = gtk_fixed_new();
    create_infofaces();
    gtk_container_add(GTK_CONTAINER(Infowind), Infolayout);

//    GtkWidget *combobox;
//    combobox = create_combobox_with_model();

    gtk_widget_set_events(Infobackg_event_box,  // 设置窗体获取鼠标事件

            GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK

                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);
    g_signal_connect(G_OBJECT(Infobackg_event_box), "button_press_event",
            G_CALLBACK(Infobackg_button_press_event), NULL);

    gtk_widget_set_events(Save_event_box,  // 设置窗体获取鼠标事件

            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK

                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK

                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

    g_signal_connect(G_OBJECT(Save_event_box), "button_press_event",
            G_CALLBACK(save_button_press_event), NULL);       // 加入事件回调
    g_signal_connect(G_OBJECT(Save_event_box), "enter_notify_event",
            G_CALLBACK(save_enter_notify_event), NULL);
    g_signal_connect(G_OBJECT(Save_event_box), "button_release_event",
            G_CALLBACK(save_button_release_event), NULL);
    g_signal_connect(G_OBJECT(Save_event_box), "leave_notify_event",
            G_CALLBACK(save_leave_notify_event), NULL);

    gtk_widget_set_events(Cancel_event_box,  // 设置窗体获取鼠标事件

            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK

                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK

                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

    g_signal_connect(G_OBJECT(Cancel_event_box), "button_press_event",
            G_CALLBACK(cancel_button_press_event), NULL);       // 加入事件回调
    g_signal_connect(G_OBJECT(Cancel_event_box), "enter_notify_event",
            G_CALLBACK(cancel_enter_notify_event), NULL);
    g_signal_connect(G_OBJECT(Cancel_event_box), "button_release_event",
            G_CALLBACK(cancel_button_release_event), NULL);
    g_signal_connect(G_OBJECT(Cancel_event_box), "leave_notify_event",
            G_CALLBACK(cancel_leave_notify_event), NULL);

    gtk_widget_set_events(Guanxx_event_box,  // 设置窗体获取鼠标事件

            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK

                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK

                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

    g_signal_connect(G_OBJECT(Guanxx_event_box), "button_press_event",
            G_CALLBACK(guanxx_button_press_event), NULL);       // 加入事件回调
    g_signal_connect(G_OBJECT(Guanxx_event_box), "enter_notify_event",
            G_CALLBACK(guanxx_enter_notify_event), NULL);
    g_signal_connect(G_OBJECT(Guanxx_event_box), "button_release_event",
            G_CALLBACK(guanxx_button_release_event), NULL);
    g_signal_connect(G_OBJECT(Guanxx_event_box), "leave_notify_event",
            G_CALLBACK(guanxx_leave_notify_event), NULL);

    gtk_widget_show_all(Infowind);
}