#include <gtk/gtk.h>
#include <ftlist.h>
#include "PopupWinds.h"

/*GtkWidget *popupLayout;
GtkWidget *popback;
cairo_surface_t *backface;
static void create_popupfaces() {
    backface = cairo_image_surface_create_from_png("提示框背景2.png");
    popback = gtk_image_new_from_surface(backface);
    gtk_fixed_put(GTK_FIXED(popupLayout), popback, 0, 0);//起始坐标

}*/
GtkWidget *box;
cairo_surface_t *backface;

void destroy(GtkWidget *widget, gpointer *data) {
    cairo_surface_destroy(backface);
    gtk_widget_destroy(box);
}

int popup(const char *title, const char *tell, GtkWindow *parent) {

    GtkWidget *popupwindow;

    GtkWidget *content, *biaoti;
    GtkWidget *ok;
    GtkWidget *popupLayout;
    GtkWidget *popback;


    //Creates a new GtkDialog
    //popupwindow = gtk_dialog_new_with_buttons(title,parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
    popupwindow = gtk_dialog_new();

    //gtk_window_set_default_size(GTK_WINDOW(popupwindow), 250, 230);
    gtk_window_set_position(GTK_WINDOW(popupwindow), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (popupwindow), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(popupwindow), FALSE);//去掉边框
    //gtk_widget_set_size_request(GTK_WIDGET(popupwindow), 250, 30);//窗口大小

    box = gtk_dialog_get_content_area((GtkDialog *) popupwindow);//得到dialog的box
    //gtk_container_set_border_width(GTK_CONTAINER(box),10);

    popupLayout = gtk_fixed_new();
    //create_popupfaces();
    backface = cairo_image_surface_create_from_png("提示框背景2.png");
    popback = gtk_image_new_from_surface(backface);
    gtk_fixed_put(GTK_FIXED(popupLayout), popback, 0, 0);//起始坐标

    ok = gtk_button_new_with_label("确 认");
    gtk_fixed_put(GTK_FIXED(popupLayout), ok, 190, 200);
    g_signal_connect(G_OBJECT(ok), "clicked", G_CALLBACK(destroy), NULL);
    //gtk_container_add(GTK_CONTAINER(popupLayout),ok);
    //gtk_signal_connect(GTK_OBJECT(ok),"clicked",G_CALLBACK(destroy),NULL);
    //gtk_box_pack_start((GtkBox *)box, ok, TRUE, TRUE, 200);

    biaoti = gtk_label_new(title);//标题
    content = gtk_label_new(tell);//内容

    gtk_box_pack_start(GTK_BOX(box), popupLayout, FALSE, FALSE, 0);
    gtk_fixed_put(GTK_FIXED(popupLayout), content, 77, 100);
    gtk_fixed_put(GTK_FIXED(popupLayout), biaoti, 5, 5);

    gtk_widget_show_all(popupwindow);
    gtk_dialog_run(GTK_DIALOG(popupwindow));
    gtk_widget_destroy(popupwindow);

    g_signal_connect(G_OBJECT(popupwindow), "response", G_CALLBACK(gtk_widget_destroy), NULL);
}