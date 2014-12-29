#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <cairo.h>
#include <logger.h>
#include <imcommon/friends.h>

static GtkWidget *background, *headx, *search, *friend,*closebut;
static GtkWidget *window;
static GtkWidget *treeView;
static GtkWidget *frameLayout, *MainLayout;
static GtkTreeIter iter1, iter2;
static cairo_surface_t *surfacemainbackgroud, *surfacehead2, *surfaceresearch, *surfacefriendimage,*surfaceclose51,*surfaceclose52,*surfaceclose53;
static int x = 0;
static int y = 0;
//static int treeviewflag=0;
extern CRPPacketInfoData userdata;
extern CRPPacketInfoData groupdata;
extern gchar *uidname;

static GtkTreeStore *store;
static GdkPixbuf *pixbuf;
static cairo_t *cr;
static GtkWidget *vbox;
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
           // gint h = gdk_pixbuf_get_height(pixbuf);
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

    surfacemainbackgroud = cairo_image_surface_create_from_png("主背景.png");
    surfacehead2 = cairo_image_surface_create_from_png("头像2.png");
    surfaceresearch = cairo_image_surface_create_from_png("搜索.png");
    surfacefriendimage = cairo_image_surface_create_from_png("好友.png");
   surfaceclose51 = cairo_image_surface_create_from_png("关闭按钮1.png");
    surfaceclose52 = cairo_image_surface_create_from_png("关闭按钮2.png");
    surfaceclose53 = cairo_image_surface_create_from_png("关闭按钮3.png");

    background = gtk_image_new_from_surface(surfacemainbackgroud);
    gtk_fixed_put(GTK_FIXED(MainLayout), background, 0, 0);//起始坐标

    headx = gtk_image_new_from_surface(surfacehead2 );
    gtk_fixed_put(GTK_FIXED(MainLayout), headx, 2, 10);

    search = gtk_image_new_from_surface(surfaceresearch);
    gtk_fixed_put(GTK_FIXED(MainLayout), search, 0, 140);

    friend = gtk_image_new_from_surface(surfacefriendimage);
    gtk_fixed_put(GTK_FIXED(MainLayout), friend, -10, 174);
    closebut = gtk_image_new_from_surface(surfaceclose51);
    gtk_fixed_put(GTK_FIXED(MainLayout), closebut, 247, 0);

}

static void
destroy_surfaces() {
    g_print("destroying surfaces2");
    cairo_surface_destroy(surfacemainbackgroud);
    cairo_surface_destroy(surfacehead2);
    cairo_surface_destroy(surfaceresearch);
    cairo_surface_destroy(surfacefriendimage);

}

//树状视图双击列表事件
gboolean button2_press_event(GtkWidget *widget , GdkEventButton *event, gpointer data)
{
    GtkWidget *menu = GTK_WIDGET(data);
    if(event->type == GDK_BUTTON_PRESS)
    {
        if ( event->button == 0x1)
            return FALSE;
        if ( event->button == 0x2)
            return FALSE;
        if ( event->button == 0x3)
            gtk_menu_popup(GTK_MENU(menu),NULL,NULL,NULL,NULL,event->button,event->time);
    }
    if(event->type == GDK_2BUTTON_PRESS && event->button == 0x1)
    {
        GtkTreeIter iter;
        GtkWidget  *treeview = GTK_TREE_VIEW(widget);
        GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
        GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
        gtk_tree_selection_get_selected(selection, &model, &iter);
        if(gtk_tree_model_iter_has_child (model, &iter)==0)
        {
                mainchart();
        }

    }
    return FALSE;
}
//鼠标点击事件
static gint button_press_event(GtkWidget *widget,GdkEventButton *event, gpointer data){
    x = event->x;  // 取得鼠标相对于窗口的位置
    y = event->y;

    if (event->button == 1 && (x > 247 && x < 280) && (y> 2 && y < 25)) {              //设置关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) closebut, surfaceclose52); //置换图标
    }
    else{
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

        gpointer data){

    x = event->x;  // 取得鼠标相对于窗口的位置
    y = event->y;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *)closebut, surfaceclose51);  //设置关闭按钮
        if ((x > 247 && x < 280) && (y > 2 && y < 25)) {
            destroy_surfaces();
            DeleteEvent();
        }
    }

    return 0;
}

//鼠标移动事件
static gint motion_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data){

    x = event->x;  // 取得鼠标相对于窗口的位置
    y = event->y;
    if ((x > 247 && x < 280) && (y > 2 && y < 25)){
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
        gtk_image_set_from_surface((GtkImage *) closebut, surfaceclose53);
    }
    else {
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
        gtk_image_set_from_surface((GtkImage *) closebut,surfaceclose51);
    }

    return 0;
}

int maininterface() {

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

    create_surfaces();
    GtkWidget *userid;
    //g_print(userdata.nickName);
    userid = gtk_label_new(userdata.nickName);
    //g_print(groupdata.nickName);
//    log_info("组员信息", groupdata.nickName);
    //g_print(groupdata.nickName);
    log_info("组员信息", groupdata.nickName);
    gtk_fixed_put(GTK_FIXED(MainLayout), userid, 170, 90);

    gtk_container_add(GTK_CONTAINER(window), frameLayout);//frameLayout 加入到window
    gtk_container_add(GTK_CONTAINER(frameLayout), MainLayout);


    treeView = gtk_tree_view_new_with_model(createModel());//list
    //gtk_tree_view_column_set_resizable(column,TRUE);//加了就bug了
    gtk_tree_view_set_headers_visible(treeView,0);//去掉头部空白

    //添加树形视图
    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_column_new_with_attributes(NULL, renderer,
            "pixbuf", PIXBUF_COL,
            NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (treeView), column);
    gtk_tree_view_column_set_resizable(column,TRUE);
    //添加滚动条
    GtkScrolledWindow *sw= gtk_scrolled_window_new(NULL,NULL);
    //设置滚动条常在状态
    gtk_scrolled_window_set_policy (sw,
            GTK_POLICY_ALWAYS,
            GTK_POLICY_ALWAYS);
    //获取水平滚动条
    GtkWidget* widget=gtk_scrolled_window_get_hscrollbar(sw);

    gtk_container_add(GTK_CONTAINER(sw), treeView);
    gtk_fixed_put(GTK_FIXED(MainLayout), sw, 0, 225);
    gtk_widget_set_size_request(sw, 284, 358);
    // 设置窗体获取鼠标事件
    gtk_widget_set_events(window,

            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK

                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK

                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

    gtk_widget_set_events(treeView, GDK_BUTTON_PRESS_MASK);

    g_signal_connect(G_OBJECT(window), "button_press_event",
            G_CALLBACK(button_press_event), window);       // 加入事件回调
    g_signal_connect(G_OBJECT(window), "motion_notify_event",
            G_CALLBACK(motion_notify_event), window);

    g_signal_connect(G_OBJECT(window), "button_release_event",
            G_CALLBACK(button_release_event), window);
//
//    g_signal_connect(G_OBJECT(treeView), "button_press_event",
//            G_CALLBACK(button_press_event), treeView);
//
    g_signal_connect(G_OBJECT(treeView), "button_press_event",
            G_CALLBACK(button2_press_event), treeView);


    gtk_widget_show_all(window);
    //隐藏水平滚动条
    gtk_widget_hide(widget);
    //gtk_main();

    return 0;
}