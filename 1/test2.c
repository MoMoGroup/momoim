#include <gtk/gtk.h>
enum{
    PIXBUF_COL,
    TEXT_COL
};
GtkTreeModel* createModel()
{
    const gchar *files[]={"王大锤.jpg","2.png","3.png","4.png"};
    GdkPixbuf *pixbuf;
    GtkTreeIter iter;
    GtkListStore *store;
    gint i;

    store = gtk_list_store_new(2,GDK_TYPE_PIXBUF, G_TYPE_STRING);   //2列

    for(i=0;i<4;i++){
        pixbuf = gdk_pixbuf_new_from_file(files[i],NULL);
        gtk_list_store_append(store,&iter);
        gtk_list_store_set(store,&iter,
                PIXBUF_COL,pixbuf,     //PIXBUF_COL也可以用0代替
                TEXT_COL,files[i],
                -1);
        gdk_pixbuf_unref(pixbuf);
    }//for
    return GTK_TREE_MODEL(store);
}
gboolean selection_changed(GtkTreeSelection *selection, GtkLabel *label){
    GtkTreeView *treeView;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *active;
    treeView = gtk_tree_selection_get_tree_view(selection);
    model=gtk_tree_view_get_model(treeView);
    gtk_tree_selection_get_selected(selection,&model,&iter);
    gtk_tree_model_get(model,&iter,1,&active,-1);
    gtk_label_set_text(label,active);
}

int main(int argc, char *argv[]){
    GtkWidget *window;
    GtkWidget *treeView;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkWidget *label;
    GtkWidget *vbox;
    GtkTreeSelection *selection;
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"GtkTreeView");
    gtk_window_set_default_size(GTK_WINDOW(window),200,50);

    treeView = gtk_tree_view_new_with_model(createModel());

    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_column_new_with_attributes("Icon",renderer, "pixbuf", PIXBUF_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Filename",renderer, "text", TEXT_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);

    label = gtk_label_new("caterpillar.jpg");
    vbox = gtk_box_new(FALSE,5);

    gtk_box_pack_start(GTK_BOX(vbox),treeView, TRUE,TRUE,5);
    gtk_box_pack_start(GTK_BOX(vbox),label, TRUE,TRUE,5);
    gtk_container_add(GTK_CONTAINER(window),vbox);
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView));
    g_signal_connect(G_OBJECT(selection), "changed",G_CALLBACK(selection_changed),label);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}