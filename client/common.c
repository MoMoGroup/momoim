#include <gtk/gtk.h>
#include "common.h"

GtkEventBox *BuildEventBox(GtkWidget *warp, GCallback press, GCallback enter, GCallback leave, GCallback release, void *data)
{
    GtkEventBox *eventBox = GTK_EVENT_BOX(gtk_event_box_new());
    gtk_widget_set_events(GTK_WIDGET(eventBox),  // 设置窗体获取鼠标事件
            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK
                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);
    if (press)
        g_signal_connect(G_OBJECT(eventBox), "button_press_event",
                G_CALLBACK(press), NULL);       // 加入事件回调
    if (enter)
        g_signal_connect(G_OBJECT(eventBox), "enter_notify_event",
                G_CALLBACK(enter), NULL);
    if (release)
        g_signal_connect(G_OBJECT(eventBox), "button_release_event",
                G_CALLBACK(release), NULL);
    if (leave)
        g_signal_connect(G_OBJECT(eventBox), "leave_notify_event",
                G_CALLBACK(leave), NULL);
    GdkRGBA rgba = {1, 1, 1, 0};
    gtk_widget_override_background_color(GTK_WIDGET(eventBox), GTK_STATE_FLAG_NORMAL, &rgba);//设置透明
    gtk_container_add((GTK_CONTAINER(eventBox)), warp);
    return eventBox;
}