#include <gtk/gtk.h>
#include<cairo.h>
#include <ftrender.h>



///* 用于list view */
//enum ListCols
//{
//    LIST_NUM,
//    LIST_NAME,
//    LIST_CHECKED,
//    LIST_CNT
//};

/* 用于tree view */
enum TreeCols {
    TREE_NAME,
    TREE_CNT
};

/* tree view选项项改变时调用的回调函数 */
static void
tree_select_changed_cb(GtkTreeSelection *select, gpointer data)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *str;

    if (gtk_tree_selection_get_selected(select, &model, &iter))
    {
        gtk_tree_model_get(model, &iter, TREE_NAME, &str, -1);
        gtk_statusbar_push(GTK_STATUSBAR(data),
                gtk_statusbar_get_context_id(GTK_STATUSBAR(data),
                        str), str);
        g_free(str);
    }
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *statusbar;
    GtkTreeView *tree;
    GtkTreeView *list;
    GtkTreeStore *tree_store;
    GtkListStore *list_store;
    GtkTreeIter iter;
    GtkTreeIter iter_child;
    //GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkTreeSelection *select;

    /* 初始化gtk */
    gtk_init(&argc, &argv);
    /* 创建窗口并设置其参数 */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //gtk_window_set_title (GTK_WINDOW (win), "My GtkTreeView");
    gtk_window_set_position(GTK_WINDOW (window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(window, 180, 400);

    /* 创建垂直布局构件 */
    box = gtk_box_new(FALSE, 3);
    gtk_container_add(GTK_CONTAINER (window), box);
    /* 创建TreeView */
    tree = gtk_tree_view_new();
    /* 创建TreeView的Model, 添加数据 */

    tree_store = gtk_tree_store_new(TREE_CNT, G_TYPE_STRING);
    gtk_tree_store_append(tree_store, &iter, NULL);

    gtk_tree_store_set(tree_store, &iter,//添加分组1
            TREE_NAME, "分组1", -1);

    gdk_pixbuf_new_from_file_at_size("1.jpg", 40, 40, NULL);

    gtk_tree_store_append(tree_store, &iter_child, &iter);
    gtk_tree_store_set(tree_store, &iter_child,
            TREE_NAME, "好友1", -1);
    gtk_tree_store_append(tree_store, &iter_child, &iter);
    gtk_tree_store_set(tree_store, &iter_child,
            TREE_NAME, "好友2", -1);
    gtk_tree_store_append(tree_store, &iter_child, &iter);
    gtk_tree_store_set(tree_store, &iter_child,
            TREE_NAME, "好友3", -1);
    gtk_tree_store_append(tree_store, &iter_child, &iter);
    gtk_tree_store_set(tree_store, &iter_child,
            TREE_NAME, "好友33", -1);

    // cairo_t *cr; //声明一支画笔
    //  cr= gdk_cairo_create(<#(GdkWindow*)window#>);//创建画笔
//
//    cairo_set_target_drawable(cr,,CAIRO_FORMAT_ARGB32,400,400);//设置画布
//    cairo_move_to(cr, 20, 20);
//    cairo_set_font_size(cr, 30);
//    cairo_select_font_face(cr,"DongWen--Song", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
//    cairo_show_text(cr, "aaa");

    // cairo_t *cr= gdk_cairo_create(win);

    cairo_t *cr; //声明一支画笔
    cr = gdk_cairo_create(gtk_widget_get_root_window(GTK_WINDOW(window)));//创建画笔
    cairo_surface_t *draw;
    draw = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 30);//创建画布
    cairo_move_to(cr, 20, 20);
    cairo_set_font_size(cr, 30);
    cairo_select_font_face(cr, "DongWen--Song", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_show_text(cr, "aaa");

    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 100, 50);
    cairo_surface_t *surface= gdk_cairo_surface_create_from_pixbuf(pixbuf, 1, gtk_widget_get_root_window(window));
    cairo_t *c= cairo_create(surface);
    cairo_move_to(cr, 0, 0);
    cairo_set_font_size(cr, 30);
    cairo_select_font_face(cr, "DongWen--Song", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_show_text(cr, "aaaa");
    pixbuf = gdk_pixbuf_get_from_surface(surface, 50, 20, 100, 50);

//    cairo_t *cr;
//    GdkWindow *window;
//    cr= gdk_cairo_create(window);
//    cairo_move_to(cr, 20, 20);
//    cairo_set_font_size(cr, 30);
//    cairo_select_font_face(cr,"DongWen--Song", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
//    cairo_show_text(cr, "aaa");



    gtk_tree_store_append(tree_store, &iter, NULL);//添加分组2

    gtk_tree_store_set(tree_store, &iter,
            TREE_NAME, "分组2", -1);
    gtk_tree_store_append(tree_store, &iter_child, &iter);

    gtk_tree_store_set(tree_store, &iter_child,
            TREE_NAME, "好友4", -1);
    gtk_tree_store_append(tree_store, &iter_child, &iter);

    gtk_tree_store_set(tree_store, &iter_child,
            TREE_NAME, "好友5", -1);
    gtk_tree_store_append(tree_store, &iter_child, &iter);
    gtk_tree_store_set(tree_store, &iter_child,
            TREE_NAME, "好友6", -1);

    gtk_tree_store_append(tree_store, &iter, NULL);//添加分组3
    gtk_tree_store_set(tree_store, &iter, TREE_NAME, "分组3", -1);

    gtk_tree_store_append(tree_store, &iter_child, &iter);
    gtk_tree_store_set(tree_store, &iter_child,
            TREE_NAME, "好友7", -1);


    gtk_tree_view_set_model(tree, tree_store);
    g_object_unref(tree_store);

    /* 创建TreeView的View */
    GtkCellRenderer* renderer =gtk_cell_renderer_pixbuf_new();
    //g_object_set_data(renderer, "pixbuf", pixbuf);
    //g_object_set_data(renderer, "surface", surface);

    column = gtk_tree_view_column_new_with_attributes(
            "操作系统(选择项改变时状态栏会响应)",
            renderer,
            "pixbuf", pixbuf,
            "surface", surface,
            NULL);
    gtk_tree_view_append_column(tree, column);
    gtk_tree_view_expand_all(tree);
    gtk_box_pack_start(box, tree, TRUE, TRUE, 1);

//    /* 创建ListView */
//    list = gtk_tree_view_new();
//    /* 创建ListView的model, 添加数据 */
//    list_store = gtk_list_store_new(LIST_CNT,
//            G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
//    gtk_list_store_append(list_store, &iter);
//    gtk_list_store_set(list_store, &iter,
//            LIST_NUM, "1001",
//            LIST_NAME, "Lucy",
//            LIST_CHECKED, FALSE, -1);
//    gtk_list_store_append(list_store, &iter);
//    gtk_list_store_set(list_store, &iter,
//            LIST_NUM, "1001",
//            LIST_NAME, "韩梅梅",
//            LIST_CHECKED, TRUE, -1);
//    gtk_list_store_append(list_store, &iter);
//    gtk_list_store_set(list_store, &iter,
//            LIST_NUM, "1001",
//            LIST_NAME, "Tom Green",
//            LIST_CHECKED, TRUE, -1);
//    gtk_list_store_append(list_store, &iter);
//    gtk_list_store_set(list_store, &iter,
//            LIST_NUM, "1001",
//            LIST_NAME, "李明",
//            LIST_CHECKED, FALSE, -1);
//    gtk_tree_view_set_model(list, list_store);
//    g_object_unref(list_store);

//    /* 创建ListView的View */
//    renderer = gtk_cell_renderer_text_new();
//    column = gtk_tree_view_column_new_with_attributes("学号", renderer,
//            "text", LIST_NUM, NULL);
//    column = gtk_tree_view_append_column(list, column);
//    renderer = gtk_cell_renderer_text_new();
//    column = gtk_tree_view_column_new_with_attributes("姓名", renderer,
//            "text", LIST_NAME, NULL);
//    column = gtk_tree_view_append_column(list, column);
//    renderer = gtk_cell_renderer_toggle_new();
//    column = gtk_tree_view_column_new_with_attributes("是否优秀?", renderer,
//            "active", LIST_CHECKED, NULL);
//    column = gtk_tree_view_append_column(list, column);
//    gtk_box_pack_start(box, list, TRUE, TRUE, 1);

//    /* 创建状态栏 */
//    statusbar = gtk_statusbar_new();
//    gtk_box_pack_start(box, statusbar, FALSE, TRUE, 1);
//
//    /* 设置tree view选择项改变时的事件处理函数 */
//    select = gtk_tree_view_get_selection(tree);
//    gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
//    g_signal_connect(G_OBJECT(select), "changed",
//            G_CALLBACK(tree_select_changed_cb), statusbar);
//    /* 窗口关闭处理 */
//    g_signal_connect (win, "destroy", gtk_main_quit, NULL);

    /* 主循环 */
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}