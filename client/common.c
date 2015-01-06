#include <gtk/gtk.h>
#include <stdint.h>
#include <protocol/base.h>
#include "common.h"

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


session_id_t CountSessionId()
{
    static session_id_t OnlySessionId = 1000;
    session_id_t ret;
    pthread_rwlock_wrlock(&onllysessionidlock);//写锁定
    ret = ++OnlySessionId;
    pthread_rwlock_unlock(&onllysessionidlock);//取消锁
    return ret;
}