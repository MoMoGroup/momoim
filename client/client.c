#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <logger.h>
#include <gtk/gtk.h>
#include <protocol.h>

void *sendhello(void *M)
{
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
    CRPLoginLoginSend(sockfd, 5, "12345", "1234567890123456");
    if (header->packetID != CRP_PACKET_OK) {
        log_error("Login", "Recv Packet:%d\n", header->packetID);
        return 1;
    }
    log_info("Login", "Waiting OK\n");
    header = CRPRecv(sockfd);
    if (header->packetID != CRP_PACKET_OK) {
        log_error("Login", "Recv Packet:%d\n", header->packetID);
        return 1;
    }
    else {
        log_info("Login", "Login Done\n");
    }
    return 0;
}

void on_button_clicked(GtkWidget *button, gpointer userdata) {

    pthread_t mythread;
    void *retu;
    pthread_create(&mythread, NULL, sendhello, NULL);


    GtkWidget *dialog;
//创建带确认按钮的对话框，父控件为空
    dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            (gchar *) userdata);
    gtk_dialog_run(GTK_DIALOG(dialog));//显示并运行对话框
    gtk_widget_destroy(userdata);//销毁对话框
    pthread_join(mythread, &retu);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *layout;
    GtkWidget *image1, *image2, *image3, *image4, *image5, *image6, *image7, *image8;
    GtkWidget *button;

    //初始化GTK+程序
    gtk_init(&argc, &argv);
    //创建窗口，并为窗口的关闭信号加回调函数以便退出
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 283, 411);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);//窗口出现位置
    // gtk_window_set_resizable (GTK_WINDOW (window), FALSE);//窗口不可改变



    layout = gtk_layout_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER (window), layout);


    image1 = gtk_image_new_from_file("背景.png");
    gtk_layout_put(GTK_LAYOUT(layout), image1, 0, 0);//起始坐标

    image2 = gtk_image_new_from_file("头像.png");
    gtk_layout_put(GTK_LAYOUT(layout), image2, 65, 30);

    image3 = gtk_image_new_from_file("白色.png");
    gtk_layout_put(GTK_LAYOUT(layout), image3, 25, 200);

    image4 = gtk_image_new_from_file("登陆按钮.png");
    //gtk_layout_put(GTK_LAYOUT(layout), image4, 70, 300);

    image5 = gtk_image_new_from_file("账号.png");
    gtk_layout_put(GTK_LAYOUT(layout), image5, 35, 220);

    image6 = gtk_image_new_from_file("密码.png");
    gtk_layout_put(GTK_LAYOUT(layout), image6, 35, 260);


    GtkWidget *box, *username, *passwd;
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


    button = gtk_button_new();

    gtk_button_set_image(button, image4);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_button_clicked), (gpointer) "你好，\n世界！");
    gtk_layout_put(GTK_LAYOUT(layout), button, 70, 300);
    gtk_button_set_relief(button, GTK_RELIEF_NONE);

//   button = gtk_button_new_with_label("登陆");
//   g_signal_connect(G_OBJECT(button), "clicked",
//            G_CALLBACK(on_button_clicked), (gpointer) "你好，\n世界！");
//    gtk_layout_put(GTK_LAYOUT(layout), button, 70, 300);
//   gtk_widget_set_size_request(button, 130, 40);//按钮大小

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}  