#include <gtk/gtk.h>

gboolean drag = FALSE;   // 只在左键按下时拖动窗体
int nX = 0;
int nY = 0;
GtkWidget *window;
GtkWidget *layout;
GtkWidget * image4,*image8;

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

static gint button_press_event(GtkWidget * widget,

        GdkEventButton * event, gpointer data)

{
    nX = event->x;  // 取得鼠标相对于窗口的位置

    nY = event->y;
    if (event->button == 1&&(nX>75&&nX<205)&&(nY>302&&nY<335))       // 判断是否左键按下

    {
        drag = TRUE;

        gtk_image_set_from_file((GtkImage *)image4, "登陆按钮2.png");
    }

    if (event->button == 1&&(nX>260&&nX<283)&&(nY>=0&&nY<10))       // 判断是否左键按下

    {
        drag = TRUE;

        gtk_image_set_from_file((GtkImage *)image8, "关闭2.png");
    }
    return TRUE;

}

static gint button_release_event(GtkWidget * widget, GdkEventButton * event,

        gpointer data)         // 鼠标抬起事件

{

    if (event->button == 1)
    {
       gtk_image_set_from_file((GtkImage *) image4, "登陆按钮.png");
        gtk_image_set_from_file((GtkImage *)image8, "关闭2.png");
        drag = FALSE;
    }
    return TRUE;

}

static gint motion_notify_event(GtkWidget * widget, GdkEventButton * event,

        gpointer data)         // 鼠标移动事件

{

    if (drag)

    {

        int x, y;

        GtkWidget *window = (GtkWidget *) data;

        gtk_window_get_position((GtkWindow *) window, &x, &y);         // 取窗体绝对坐标

        gtk_window_move((GtkWindow *) window, x + event->x - nX, //x是当前窗口的绝对坐标，event->x是鼠标相对窗口的x坐标， nX是鼠标按下时记录的坐标，这三个量都是不变的，，窗口怎么会移动呢？？？？


                y + event->y - nY);// 移动窗体

    }

    return TRUE;

}


int main( int argc, char *argv[])
{
    GtkWidget *image1,*image2,*image3,*image5,*image6,*image7;

    //初始化GTK+程序
    gtk_init(&argc, &argv);
    //创建窗口，并为窗口的关闭信号加回调函数以便退出
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window),"delete_event",
            G_CALLBACK(gtk_main_quit),NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 283, 411);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);//窗口出现位置
    // gtk_window_set_resizable (GTK_WINDOW (window), FALSE);//窗口不可改变

    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);   // 去掉边框

    gtk_widget_set_events(window,  // 设置窗体获取鼠标事件

            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK

                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK

                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

    g_signal_connect(G_OBJECT(window), "button_press_event",

            G_CALLBACK (button_press_event), window);       // 加入事件回调

    g_signal_connect(G_OBJECT(window), "motion_notify_event",

            G_CALLBACK (motion_notify_event), window);

    g_signal_connect(G_OBJECT(window), "button_release_event",

            G_CALLBACK (button_release_event), window);


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

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}