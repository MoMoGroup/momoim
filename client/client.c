#include <gtk/gtk.h>  

void	on_button_clicked(GtkWidget* button, gpointer userdata)
{
    GtkWidget *dialog;
//创建带确认按钮的对话框，父控件为空
    dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL |GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            (gchar*)userdata);
    gtk_dialog_run(GTK_DIALOG(dialog));//显示并运行对话框
    gtk_widget_destroy(dialog);//销毁对话框
}

int main( int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *layout;
    GtkWidget *image1,*image2,*image3,*image4,*image5,*image6,*image7,*image8;
    GtkWidget *button;

    //初始化GTK+程序
    gtk_init(&argc, &argv);
    //创建窗口，并为窗口的关闭信号加回调函数以便退出
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window),"delete_event",
            G_CALLBACK(gtk_main_quit),NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 283, 411);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);//窗口出现位置
   // gtk_window_set_resizable (GTK_WINDOW (window), FALSE);//窗口不可改变

    layout = gtk_layout_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER (window), layout);
    gtk_widget_show(layout);

    image1 = gtk_image_new_from_file("背景.png");
    gtk_layout_put(GTK_LAYOUT(layout), image1, 0, 0);//起始坐标

    image2 = gtk_image_new_from_file("头像.png");
    gtk_layout_put(GTK_LAYOUT(layout), image2, 65, 30);

    image3 = gtk_image_new_from_file("白色.png");
    gtk_layout_put(GTK_LAYOUT(layout), image3, 25, 200);

    image4 = gtk_image_new_from_file("登陆按钮.png");
    gtk_layout_put(GTK_LAYOUT(layout), image4, 70, 300);

    image5 = gtk_image_new_from_file("账号.png");
    gtk_layout_put(GTK_LAYOUT(layout), image5, 35, 220);

    image6 = gtk_image_new_from_file("密码.png");
    gtk_layout_put(GTK_LAYOUT(layout), image6, 35, 260);

    image7 = gtk_image_new_from_file("注册账号.png");
    gtk_layout_put(GTK_LAYOUT(layout), image7,5, 380);

    image8 = gtk_image_new_from_file("关闭.png");
    gtk_layout_put(GTK_LAYOUT(layout), image8, 260, 0);

//    button=gtk_button_new_with_label("登陆");
//    g_signal_connect(G_OBJECT(button),"clicked",
//            G_CALLBACK(on_button_clicked),(gpointer)"你好，\n世界！");
//    gtk_layout_put(GTK_LAYOUT(layout), button, 70, 300);
//    gtk_widget_set_size_request(button, 130, 40);//按钮大小

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}  