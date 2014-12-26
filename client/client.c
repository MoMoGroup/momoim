#include <gtk/gtk.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <protocol/status/Hello.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include<openssl/md5.h>
#include <stdlib.h>

//#define NAME    "root"
//#define PWD     "123456"
GtkWidget *username, *passwd;
gboolean drag = FALSE;   // 只在左键按下时拖动窗体
int nX = 0;
int nY = 0;
GtkWidget *window;
GtkWidget *layout;
GtkWidget *image4, *image8;

void *sendhello(void *M) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
            .sin_family=AF_INET,
            .sin_addr.s_addr=htonl(INADDR_LOOPBACK),
            .sin_port=htons(8014)
    };
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        perror("Connect");
        return 1;
    }
    log_info("Hello", "Sending Hello\n");
    CRPHelloSend(sockfd, 1, 1, 1);
    CRPBaseHeader *header;
    log_info("Hello", "Waiting OK\n");
    header = CRPRecv(sockfd);
    if (header->packetID != CRP_PACKET_OK) {
        log_error("Hello", "Recv Packet:%d\n", header->packetID);
        return 1;
    }

    log_info("Login", "Sending Login Request\n");
    gchar *name, *pwd;
    name = gtk_entry_get_text(GTK_ENTRY(username));
    pwd = gtk_entry_get_text(GTK_ENTRY(passwd));
    log_info("Login", "Sending Login Request\n");
    unsigned char hash[16];

    MD5((unsigned char *) pwd, 1, hash);
    CRPLoginLoginSend(sockfd, name, hash);

    //CRPLoginLoginSend(sockfd, "12345", "1234567890123456");

    return 0;
}

void on_button_clicked() {
    pthread_t mythread;
    void *retu;
    pthread_create(&mythread, NULL, sendhello, NULL);

    gtk_widget_show(window);


    gtk_widget_destroy(layout);//销毁layout对话框

    GtkWidget *image1, *image2, *image3, *image4, *layout2;
    layout2 = gtk_layout_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER (window), layout2);
    image1 = gtk_image_new_from_file("背景.png");
    gtk_layout_put(GTK_LAYOUT(layout2), image1, 0, 0);//起始坐标

    image2 = gtk_image_new_from_file("狗狗.png");
    gtk_layout_put(GTK_LAYOUT(layout2), image2, 45, 150);

    image3 = gtk_image_new_from_file("玩命登陆.png");
    gtk_layout_put(GTK_LAYOUT(layout2), image3, 40, 60);
    gtk_widget_show_all(layout2);


    pthread_join(mythread, &retu);
}

static gint button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data) {
    nX = event->x;  // 取得鼠标相对于窗口的位置

    nY = event->y;
    if (event->button == 1 && (nX > 75 && nX < 205) && (nY > 302 && nY < 335))       // 判断是否左键按下

    {
        drag = TRUE;

        gtk_image_set_from_file((GtkImage *) image4, "登陆按钮2.png");

        // gtk_layout_put(GTK_LAYOUT(layout), event->button, 70, 300);
        // gtk_button_set_relief(event->button, GTK_RELIEF_NONE);
    }

//    if (event->button == 1&&(nX>260&&nX<283)&&(nY>=0&&nY<10))       // 判断是否左键按下
//
//    {
//        drag = TRUE;
//
//        gtk_image_set_from_file((GtkImage *)image8, "关闭2.png");
//
//
//    }
    return TRUE;

}

static gint button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 鼠标抬起事件

{
    nX = event->x;  // 取得鼠标相对于窗口的位置

    nY = event->y;
    if (event->button == 1 && (nX > 75 && nX < 205) && (nY > 302 && nY < 335)) {
        gtk_image_set_from_file((GtkImage *) image4, "登陆按钮.png");
        on_button_clicked();
        //gtk_image_set_from_file((GtkImage *)image8, "关闭2.png");
        drag = FALSE;
    }
    return TRUE;

}

static gint motion_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)         // 鼠标移动事件

{

    if (drag) {

        int x, y;

        GtkWidget *window = (GtkWidget *) data;

        gtk_window_get_position((GtkWindow *) window, &x, &y);         // 取窗体绝对坐标

        gtk_window_move((GtkWindow *) window, x + event->x - nX, //x是当前窗口的绝对坐标，event->x是鼠标相对窗口的x坐标， nX是鼠标按下时记录的坐标，这三个量都是不变的，，窗口怎么会移动呢？？？？


                y + event->y - nY);// 移动窗体

    }

    return TRUE;

}


int main(int argc, char *argv[]) {
    GtkWidget *image1, *image2, *image3, *image5, *image6, *image7;

    //初始化GTK+程序
    gtk_init(&argc, &argv);
    //创建窗口，并为窗口的关闭信号加回调函数以便退出
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 283, 411);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);//窗口出现位置
    // gtk_window_set_resizable (GTK_WINDOW (window), FALSE);//窗口不可改变

    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);   // 去掉边框

    gtk_widget_set_events(window,  // 设置窗体获取鼠标事件

            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK

                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK

                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

    g_signal_connect(G_OBJECT(window), "button_press_event",

            G_CALLBACK(button_press_event), window);       // 加入事件回调

    g_signal_connect(G_OBJECT(window), "motion_notify_event",

            G_CALLBACK(motion_notify_event), window);

    g_signal_connect(G_OBJECT(window), "button_release_event",

            G_CALLBACK(button_release_event), window);


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

//    GtkWidget *username, *passwd;
    username = gtk_entry_new();
    passwd = gtk_entry_new();


    gtk_entry_set_visibility(GTK_ENTRY(passwd), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(passwd), '*');

    gtk_layout_put(GTK_LAYOUT(layout), username, 85, 220);
    gtk_layout_put(GTK_LAYOUT(layout), passwd, 85, 260);

    image7 = gtk_image_new_from_file("注册账号.png");
    gtk_layout_put(GTK_LAYOUT(layout), image7, 5, 380);

    image8 = gtk_image_new_from_file("关闭.png");
    gtk_layout_put(GTK_LAYOUT(layout), image8, 260, 0);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}