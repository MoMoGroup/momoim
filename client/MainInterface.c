#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"

extern CRPPacketInfoData userdata;
extern gchar *uidname;

int maininterface() {

    GtkWidget *background, *headx, *search, *friend;
    GtkWidget *frameLayout, *MainLayout;
    GtkWidget *window;

    //gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 284, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

    frameLayout = gtk_layout_new(NULL, NULL);
    MainLayout = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), frameLayout);//frameLayout 加入到window
    gtk_container_add(GTK_CONTAINER(frameLayout), MainLayout);

    background = gtk_image_new_from_file("主背景.png");
    gtk_fixed_put(GTK_FIXED(MainLayout), background, 0, 0);//起始坐标

    headx = gtk_image_new_from_file("头像2.png");
    gtk_fixed_put(GTK_FIXED(MainLayout), headx, 2, 10);

    search = gtk_image_new_from_file("搜索.png");
    gtk_fixed_put(GTK_FIXED(MainLayout), search, 0, 140);

    friend = gtk_image_new_from_file("好友.png");
    gtk_fixed_put(GTK_FIXED(MainLayout), friend, -10, 174);

    GtkWidget *userid;
    //g_print(userdata.nickName);
    userid = gtk_label_new(userdata.nickName);
    gtk_fixed_put(GTK_FIXED(MainLayout), userid, 170, 90);

    gtk_widget_show_all(window);
    //gtk_main();

    return 0;
}
