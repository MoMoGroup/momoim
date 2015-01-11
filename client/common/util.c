#include <gtk/gtk.h>
#include <stdint.h>
#include <protocol/base.h>
#include <glib-unix.h>
#include "common.h"
#include "../ClientSockfd.h"
#include "../MainInterface.h"
#include <string.h>
#include <pwd.h>

pthread_rwlock_t onllysessionidlock = PTHREAD_RWLOCK_INITIALIZER;

GtkEventBox *BuildEventBox(GtkWidget *warp, GCallback press, GCallback enter, GCallback leave, GCallback release,
        GCallback click, void *data)
{
    GtkEventBox *eventBox = GTK_EVENT_BOX(gtk_event_box_new());
    gtk_widget_set_events(GTK_WIDGET(eventBox),  // 设置窗体获取鼠标事件
            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK
                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);
    if (press)
        g_signal_connect(G_OBJECT(eventBox), "button_press_event", G_CALLBACK(press), data);       // 加入事件回调
    if (enter)
        g_signal_connect(G_OBJECT(eventBox), "enter_notify_event", G_CALLBACK(enter), data);
    if (release)
        g_signal_connect(G_OBJECT(eventBox), "button_release_event", G_CALLBACK(release), data);
    if (leave)
        g_signal_connect(G_OBJECT(eventBox), "leave_notify_event", G_CALLBACK(leave), data);
    if (click)
        g_signal_connect(G_OBJECT(eventBox), "clicked", G_CALLBACK(click), data);
    GdkRGBA rgba = {1, 1, 1, 0};
    gtk_widget_override_background_color(GTK_WIDGET(eventBox), GTK_STATE_FLAG_NORMAL, &rgba);//设置透明
    gtk_container_add((GTK_CONTAINER(eventBox)), warp);
    return eventBox;
}


void Md5Coding(gchar *filename, const unsigned char *coding_text)
{
    MD5_CTX c;
    char buf[512];
    ssize_t bytes;
    int fd;
    MD5_Init(&c);

    if ((fd = open(filename, O_RDONLY)) < -1)
    {
        printf("can not open filename");

    }
    else
    {
        bytes = read(fd, buf, 512);
        while (bytes > 0)
        {
            MD5_Update(&c, buf, bytes);
            bytes = read(fd, buf, 512);
        }
        MD5_Final(coding_text, &c);

    }
    close(fd);
}

int CopyFile(const char *sourceFileNameWithPath, const char *targetFileNameWithPath)
{
    FILE *fpR, *fpW;
    char buffer[256] = "\0";
    int lenR, lenW;
    if ((fpR = fopen(sourceFileNameWithPath, "r")) == NULL)
    {
        return 0;
    }

    if ((fpW = fopen(targetFileNameWithPath, "w")) == NULL)
    {
        fclose(fpR);
        return 0;
    }
    memset(buffer, 0, 256);
    while ((lenR = fread(buffer, 1, 256, fpR)) > 0)
    {
        if ((lenW = fwrite(buffer, 1, lenR, fpW)) != lenR)
        {
            fclose(fpR);
            fclose(fpW);
            return 1;
        }
    }
    fclose(fpR);
    fclose(fpW);
    return 0;
}

void HexadecimalConversion(char *filename, unsigned char const *fileKey)
{
    char sDest[33] = {0};
    short i;

    for (i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        sprintf(sDest + i * 2, "%02x", (int) fileKey[i]);
    }
    sprintf(filename, "%s/.momo/files/%s", getpwuid(getuid())->pw_dir, sDest);
}


session_id_t CountSessionId()
{
    static session_id_t OnlySessionId = 1000;
    session_id_t ret;
    pthread_rwlock_wrlock(&onllysessionidlock);//写锁定
    ret = ++OnlySessionId;
    pthread_rwlock_unlock(&onllysessionidlock);//取消锁
    return ret;
}


typedef struct find_image_recv_new {
    gboolean  (*fn)(void *data);

    void *data;
    unsigned char key[16];
    FILE *fp;

};

int recv_new_friend_image(CRPBaseHeader *header, void *data)
{
    struct find_image_recv_new *p = (struct find_image_recv_new *) data;

    switch (header->packetID)
    {
        case CRP_PACKET_FAILURE:
        {
            CRPPacketFailure *infodata = CRPFailureCast(header);
            break;
        };

        case CRP_PACKET_FILE_DATA_START:
        {
            CRPPacketFileDataStart *packet = CRPFileDataStartCast(header);

            char filename[256];
            HexadecimalConversion(filename, p->key);
            p->fp = fopen(filename, "w");

            CRPOKSend(sockfd, header->sessionID);
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            break;
        };

        case CRP_PACKET_FILE_DATA://接受头像
        {
            CRPPacketFileData *packet = CRPFileDataCast(header);

            fwrite(packet->data, 1, packet->length, p->fp);
            CRPOKSend(sockfd, header->sessionID);
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            break;
        };
        case CRP_PACKET_FILE_DATA_END://头像接受完
        {

            CRPPacketFileDataEnd *packet = CRPFileDataEndCast(header);
            fclose(p->fp);
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            g_idle_add(p->fn, p->data);
            free(p);
            return 0;
        }

    }
}

int FindImage(const char *key, const void *data, gboolean (*fn)(void *data))
{
    char filaname[256];
    HexadecimalConversion(filaname, key);//计算一个文件名
    //0存在，1不存在
    if (access(filaname, F_OK))//不存在，先加载图片
    {
        struct find_image_recv_new *p = malloc(sizeof(struct find_image_recv_new));
        memcpy(p->key, key, 16);
        p->fn = fn;
        p->data = data;
        //注册一个接收新添加好友头像的会话
        session_id_t sessionid = CountSessionId();
        AddMessageNode(sessionid, recv_new_friend_image, p);
        CRPFileRequestSend(sockfd, sessionid, 0, key);//发送用户头像请求
    }
    else
    {
        g_idle_add(fn, data);//执行更新函数
    }

}