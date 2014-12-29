#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <cairo.h>
#include <logger.h>
#include <imcommon/friends.h>

GtkWidget *background, *headx, *search, *friend,*closebut;
GtkWidget *window;
GtkWidget *frameLayout, *MainLayout;
cairo_surface_t *surface1, *surface2, *surface3, *surface4,*surface51,*surface52,*surface53;
int X = 0;
int Y = 0;

extern CRPPacketInfoData userdata;
extern CRPPacketInfoData groupdata;
extern gchar *uidname;


GdkPixbuf *pixbuf;
cairo_t *cr;
GtkWidget *vbox;
enum {
    PIXBUF_COL,
};


GtkTreeModel *createModel() {
    char *files[] = {
            "分组1",
            "分组2",
            "分组3",
            "分组4",
    };
    gchar *stocks[] = {
            "好友.png",
            "好友.png",
            "好友.png",
            "好友.png",
            "好友.png",
    };

    gchar *stockNames[] = {
            "头像",
            "头像2",
            "头像",
            "头像2",
            "头像",
    };

    GtkTreeIter iter1, iter2;
    GtkTreeStore *store;
    gint i, j;
    cairo_surface_t *surface;
    cairo_surface_t *surfaceIcon;

    store = gtk_tree_store_new(1, GDK_TYPE_PIXBUF);
    for (i = 0; i < 4; i++) {
        surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 260, 33);
        cr = cairo_create(surface);
        cairo_move_to(cr, 0, 20);
        cairo_set_font_size(cr, 14);
        cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_show_text(cr, files[i]);
        pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, 260, 33);
        gtk_tree_store_append(store, &iter1, NULL);
        gtk_tree_store_set(store, &iter1,
                PIXBUF_COL, pixbuf,
                -1);

        gdk_pixbuf_unref(pixbuf);

        for (j = 0; j < 4; j++) {
            pixbuf = gdk_pixbuf_new_from_file(stocks[j], NULL);
            gint w = gdk_pixbuf_get_width(pixbuf);
            gint h = gdk_pixbuf_get_height(pixbuf);
            //加载一个图片
            surfaceIcon = cairo_image_surface_create_from_png(stocks[j]);
            //创建画布
            surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 260, 60);
            //创建画笔
            cr = cairo_create(surface);
            //把画笔和图片相结合。
            cairo_set_source_surface(cr, surfaceIcon, 0, 0);
            //把图用画笔画在画布中
            cairo_paint(cr);
            //设置源的颜色
            cairo_set_source_rgb(cr, 0, 0, 0);
            //从图像的w+10,30区域开始加入字体
            cairo_move_to(cr, w + 10, 30);
            cairo_set_font_size(cr, 12);
            cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
            cairo_show_text(cr, stockNames[j]);
            pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, 260, 60);
            gtk_tree_store_append(store, &iter2, &iter1);
            gtk_tree_store_set(store, &iter2,
                    PIXBUF_COL, pixbuf,
                    -1);
            gdk_pixbuf_unref(pixbuf);
        }
    }
    cairo_destroy(cr);
    return GTK_TREE_MODEL(store);
}


static void create_surfaces() {

    surface1 = cairo_image_surface_create_from_png("主背景.png");
    surface2 = cairo_image_surface_create_from_png(mulu);
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
        if ((X > 247 && X < 280) && (Y > 2 && Y < 25)) {
            destroy_surfaces();
            DeleteEvent();
        }
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

    GtkWidget *treeView;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;//列表
    vbox = gtk_box_new(TRUE, 5);

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
    //g_print(groupdata.nickName);
    log_info("组员信息", groupdata.nickName);
    gtk_fixed_put(GTK_FIXED(MainLayout), userid, 170, 90);

    gtk_container_add(GTK_CONTAINER(window), frameLayout);//frameLayout 加入到window
    gtk_container_add(GTK_CONTAINER(frameLayout), MainLayout);


    treeView = gtk_tree_view_new_with_model(createModel());//list
    //gtk_tree_view_column_set_resizable(column,TRUE);//加了就bug了
    gtk_tree_view_set_headers_visible(treeView,0);//去掉头部空白


    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_column_new_with_attributes(NULL, renderer,
            "pixbuf", PIXBUF_COL,
            NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (treeView), column);
    gtk_tree_view_column_set_resizable(column,TRUE);

    GtkScrolledWindow *sw= gtk_scrolled_window_new(NULL, NULL);

    gtk_container_add(GTK_CONTAINER(sw), treeView);
    gtk_fixed_put(GTK_FIXED(MainLayout), sw, 0, 225);
    gtk_widget_set_size_request(sw, 284, 360);

    gtk_widget_show_all(window);
    //gtk_main();

    return 0;
}