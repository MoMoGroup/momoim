#include <gtk/gtk.h>
#include <stdint.h>
#include <protocol/base.h>
#include <cairo-script-interpreter.h>
#include <glib-unix.h>
#include "common.h"
#include <string.h>
#include <pwd.h>

pthread_rwlock_t onllysessionidlock = PTHREAD_RWLOCK_INITIALIZER;

GtkEventBox *BuildEventBox(GtkWidget *warp, GCallback press, GCallback enter, GCallback leave, GCallback release, void *data)
{
    GtkEventBox *eventBox = GTK_EVENT_BOX(gtk_event_box_new());
    gtk_widget_set_events(GTK_WIDGET(eventBox),  // 设置窗体获取鼠标事件
            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK
                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);
    if (press)
        g_signal_connect(G_OBJECT(eventBox), "button_press_event",
                G_CALLBACK(press), data);       // 加入事件回调
    if (enter)
        g_signal_connect(G_OBJECT(eventBox), "enter_notify_event",
                G_CALLBACK(enter), data);
    if (release)
        g_signal_connect(G_OBJECT(eventBox), "button_release_event",
                G_CALLBACK(release), data);
    if (leave)
        g_signal_connect(G_OBJECT(eventBox), "leave_notify_event",
                G_CALLBACK(leave), data);
    GdkRGBA rgba = {1, 1, 1, 0};
    gtk_widget_override_background_color(GTK_WIDGET(eventBox), GTK_STATE_FLAG_NORMAL, &rgba);//设置透明
    gtk_container_add((GTK_CONTAINER(eventBox)), warp);
    return eventBox;
}


void Md5Coding(gchar *filename, unsigned char *coding_text)
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
    char buffer[256] = "0";
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

void HexadecimalConversion(char *filename,const char *strdest)
{
    char sDest[33] = {0};
    short i;
    char szTmp[3];

    for (i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        sprintf(szTmp, "%02x", (unsigned char) strdest[i]);
        memcpy(&sDest[i * 2], szTmp, 2);
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