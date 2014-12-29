#include <gtk/gtk.h>
#include <cairo.h>


GdkPixbuf *pixbuf;
GtkWidget *window;
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


int main(int argc, char *argv[]) {

    GtkWidget *treeView;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkTreeSelection *selection;

    gtk_init(&argc, &argv);
    vbox = gtk_box_new(TRUE, 5);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 280, 450);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);//窗口出现位置


    treeView = gtk_tree_view_new_with_model(createModel());
    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_column_new_with_attributes(NULL, renderer,
            "pixbuf", PIXBUF_COL,
            NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (treeView), column);
    gtk_box_pack_start(GTK_BOX(vbox), treeView, TRUE, TRUE, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    g_signal_connect(G_OBJECT(window), "destroy",
            G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}