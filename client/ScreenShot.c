#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include "ClientSockfd.h"

void show_picture(GdkWindow *window, FriendInfo *info) //显示截图函数
{
    GdkPixbuf *pixbuf;
    GtkWidget *image;
    gchar *filename = (gchar *) malloc(256);
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    sprintf(filename, "%s/.momo/screen/%d -%d-%d %d:%d:%d 的屏幕截图.png", getpwuid(getuid())->pw_dir,
            p->tm_year + 1900, p->tm_mon + 1, p->tm_mday,
            p->tm_hour, p->tm_min, p->tm_sec);
    gdk_window_set_cursor(gdk_get_default_root_window(),
                          gdk_cursor_new(GDK_LEFT_PTR));
    gdk_flush(); //恢复鼠标光标图案
    pixbuf = gdk_pixbuf_get_from_window(window,
                                        info->data->x,
                                        info->data->y,
                                        info->data->width,
                                        info->data->height);  //取到矩形区域图片
    gdk_pixbuf_save(pixbuf, filename, "png", NULL, NULL);
    image = gtk_image_new_from_pixbuf(pixbuf);
    GtkTextMark *mark;
    GtkTextIter iter;
    GtkTextChildAnchor *anchor;
    size_t filenamelen;
    mark = gtk_text_buffer_get_insert(info->input_buffer);
    gtk_text_buffer_get_iter_at_mark(info->input_buffer, &iter, mark);
    anchor = gtk_text_buffer_create_child_anchor(info->input_buffer, &iter); //添加衍生构件
    filenamelen = strlen(filename);
    char *pSrc = malloc(filenamelen + 1);
    memcpy(pSrc, filename, filenamelen);
    pSrc[filenamelen] = 0;
    g_object_set_data_full(G_OBJECT(image), "ImageSrc", pSrc, free); //将路径存成为key值在image控件中保存
    gtk_widget_show_all(image);
    gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW (info->input_text), image, anchor);
    gtk_widget_grab_focus(info->input_text);
    //  free(filename);
}

static gint select_area_press(GtkWidget *widget, GdkEventButton *event, JieTuDATA *data)  //鼠标按下时的操作
{
    if (data->press == TRUE)
    {
        return 0;
    } //如果当前鼠标已经按下直接返回

    gtk_window_move(GTK_WINDOW(widget), -100, -100); //将窗口移出屏幕之外
    gtk_window_resize(GTK_WINDOW(widget), 10, 10);
    gtk_window_set_opacity(GTK_WINDOW(widget), 0.1); //设置窗口透明度为80%不透明
    data->press = TRUE;
    data->x = event->x_root;
    data->y = event->y_root;  //得到当前鼠标所在坐标
    return 0;
}

static gint select_area_release(GtkWidget *widget, GdkEventButton *event, JieTuDATA *data)  //鼠标释放时操作
{
    if (!data->press)
    {
        return 0;
    }

    data->width = ABS(data->x - event->x_root);
    data->height = ABS(data->y - event->y_root); //得到当前矩形的宽度和高度
    data->x = MIN(data->x, event->x_root);
    data->y = MIN(data->y, event->y_root);  //得到当前矩形初始坐标
    data->press = FALSE;

    gtk_widget_destroy(widget);
    gtk_main_quit();
    return 0;
}

static gint select_area_move(GtkWidget *widget, GdkEventMotion *event, JieTuDATA *data) //鼠标移动时操作
{
    GdkRectangle draw;
    if (!data->press)
    {
        return 0;
    }
    draw.width = ABS(data->x - event->x_root);
    draw.height = ABS(data->y - event->y_root);
    draw.x = MIN(data->x, event->x_root);
    draw.y = MIN(data->y, event->y_root); //得到当前矩形初始坐标和当前矩形宽度

    if (draw.width <= 0 || draw.height <= 0)
    {
        gtk_window_move(GTK_WINDOW(widget), -100, -100);
        gtk_window_resize(GTK_WINDOW(widget), 10, 10);
        return 0;
    }

    gtk_window_move(GTK_WINDOW(widget), draw.x, draw.y);
    gtk_window_resize(GTK_WINDOW(widget), draw.width, draw.height);  //将窗口移动到当前矩形初始坐标处并画出窗口
    return 0;
}

void ScreenShot(FriendInfo *info)
{
    GtkWidget *win;
    GdkScreen *screen;
    GdkColor color;
    screen = gdk_screen_get_default();
    win = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_widget_set_app_paintable(win, TRUE);
    info->data = (JieTuDATA *) malloc(sizeof(JieTuDATA));
    info->data->press = FALSE;
    gtk_widget_add_events(win, GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); //添加信号
    g_signal_connect(G_OBJECT(win), "button_press_event",
                     G_CALLBACK(select_area_press), info->data);
    g_signal_connect(G_OBJECT(win), "button_release_event",
                     G_CALLBACK(select_area_release), info->data);
    g_signal_connect(G_OBJECT(win), "motion_notify_event",
                     G_CALLBACK(select_area_move), info->data);

    color.blue = 0;
    color.green = 0;
    color.red = 0;
    gtk_widget_modify_bg(win, GTK_STATE_NORMAL, &color); //设置背景

    gtk_window_set_opacity(GTK_WINDOW(win), 0.2); //设置窗口全透明
    gtk_window_resize(GTK_WINDOW(win),
                      gdk_screen_get_width(screen),
                      gdk_screen_get_height(screen)); //设置窗口大小为全屏

    gdk_window_set_cursor(gdk_get_default_root_window(),
                          gdk_cursor_new(GDK_CROSSHAIR));
    gdk_flush(); //设置并更新鼠标光标图案

    gtk_widget_show_all(win);
    gtk_main();
    usleep(30000);  //这里要等待一小会，不然截取的图像会有些残影

    show_picture(gdk_get_default_root_window(), info);

}