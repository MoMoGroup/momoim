#include <gtk/gtk.h>
#include"ClientSockfd.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <protocol/status/Hello.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include<openssl/md5.h>
#include "MainInterface.h"

extern int flag;
extern GtkWidget *loginLayout, *pendingLayout;
extern GtkWidget *username, *passwd;
extern GtkWidget *window;

gboolean mythread(gpointer user_data) {
    gtk_widget_destroy(window);
    maininterface();
    return 0;
}

int mysockfd() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
            .sin_family=AF_INET,
            .sin_addr.s_addr=htonl(INADDR_LOOPBACK),
            .sin_port=htons(8014)
    };
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        perror("Connect");
        return 0;
    }
    log_info("Hello", "Sending Hello\n");
    CRPHelloSend(sockfd, 1, 1, 1);
    CRPBaseHeader *header;
    log_info("Hello", "Waiting OK\n");
    header = CRPRecv(sockfd);
    if (header->packetID != CRP_PACKET_OK) {
        log_error("Hello", "Recv Packet:%d\n", header->packetID);
        return 0;
    }

    log_info("Login", "Sending Login Request\n");
    const gchar *name, *pwd;
    name = gtk_entry_get_text(GTK_ENTRY(username));
    pwd = gtk_entry_get_text(GTK_ENTRY(passwd));

    unsigned char hash[16];
    MD5((unsigned char *) pwd, 1, hash);
    CRPLoginLoginSend(sockfd, name, hash);//发送用户名密码


    header = CRPRecv(sockfd);
    if (header->packetID == CRP_PACKET_FAILURE) {
        //密码错误
        log_info("登录失败", "登录失败\n");
        flag = 1;
        gtk_widget_destroy(pendingLayout);
        //gtk_container_add(GTK_CONTAINER (window), layout);
        gtk_widget_show_all(loginLayout);
        return 1;
    }
    if (header->packetID == CRP_PACKET_LOGIN_ACCEPT) {
        log_info("登录成功", "登录成功\n");

        CRPPacketLoginAccept *ac = CRPLoginAcceptCast(header);
        uint32_t uid = ac->uid;
        CRPInfoRequestSend(sockfd, uid);
        header = CRPRecv(sockfd);
        if (header->packetID == CRP_PACKET_INFO_DATA) {
            log_info("User", "Nick%s\n", (CRPInfoDataCast(header)->nickName));
            gdk_threads_add_idle(mythread, NULL);
        }
        else {
            log_info("User", "Info Failure\n");
        }
        return 0;

        //销毁loginlayout对话框
        //gtk_container_add(GTK_CONTAINER(window), pendingLayout);

        //gtk_widget_show_all(pendingLayout);

    }
    return 0;
}