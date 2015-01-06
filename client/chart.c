#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <stdlib.h>
#include <ftadvanc.h>
#include "chart.h"
#include "common.h"
#include <pwd.h>
#include <math.h>

int X = 0;
int Y = 0;


static cairo_surface_t *schartbackgroud, *surfacesend1, *surfacesend2, *surfacehead3, *surfacevoice1, *surfacevoice2, *surfacevideo1, *surfacevideo2;
static cairo_surface_t *surfaceclose1, *surfaceclose2, *surfaceclosebut1, *surfaceclosebut2, *surfaceclosebut3;
static cairo_surface_t *surfacelook1, *surfacelook2, *surfacejietu1, *surfacejietu2, *surfacefile1, *surfacefile2, *surfaceimage1, *surfaceimage2;
static cairo_surface_t *surfacewordart1, *surfacewordart2;

static void create_surfaces(friendinfo *information)
{
    if (schartbackgroud == NULL)
    {
        schartbackgroud = cairo_image_surface_create_from_png("聊天背景1.png");
        surfacesend1 = cairo_image_surface_create_from_png("发送1.png");
        surfacesend2 = cairo_image_surface_create_from_png("发送2.png");

        surfacevoice1 = cairo_image_surface_create_from_png("语音1.png");
        surfacevoice2 = cairo_image_surface_create_from_png("语音2.png");
        surfacevideo1 = cairo_image_surface_create_from_png("视频1.png");
        surfacevideo2 = cairo_image_surface_create_from_png("视频2.png");

        surfacelook1 = cairo_image_surface_create_from_png("表情.png");
        surfacelook2 = cairo_image_surface_create_from_png("表情2.png");
        surfacejietu1 = cairo_image_surface_create_from_png("截图.png");
        surfacejietu2 = cairo_image_surface_create_from_png("截图2.png");
        surfacefile1 = cairo_image_surface_create_from_png("文件.png");
        surfacefile2 = cairo_image_surface_create_from_png("文件2.png");
        surfaceimage1 = cairo_image_surface_create_from_png("图片.png");
        surfaceimage2 = cairo_image_surface_create_from_png("图片2.png");
        surfacewordart1 = cairo_image_surface_create_from_png("字体.png");
        surfacewordart2 = cairo_image_surface_create_from_png("字体2.png");

        surfaceclose1 = cairo_image_surface_create_from_png("关闭1.png");
        surfaceclose2 = cairo_image_surface_create_from_png("关闭2.png");
        surfaceclosebut1 = cairo_image_surface_create_from_png("关闭按钮1.png");
        surfaceclosebut2 = cairo_image_surface_create_from_png("关闭按钮2.png");
        surfaceclosebut3 = cairo_image_surface_create_from_png("关闭按钮3.png");

    }

    static cairo_t *cr;
    char mulu[80] = {0};
    cairo_surface_t *surface;


    sprintf(mulu, "%s/.momo/friend/%u.png", getpwuid(getuid())->pw_dir, information->user.uid);

    //加载一个图片
    surface = cairo_image_surface_create_from_png(mulu);
    int w = cairo_image_surface_get_width(surface);
    int h = cairo_image_surface_get_height(surface);
    //创建画布
    surfacehead3 = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 72, 72);
    //创建画笔
    cr = cairo_create(surfacehead3);
    //缩放
    cairo_arc(cr, 36, 36, 36, 0, M_PI * 2);
    cairo_clip(cr);
    cairo_scale(cr, 72.0 / w, 72.0 / h);
    //把画笔和图片相结合。
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

//将输入的文本框输出在显示的文本框中
void show_local_text(const gchar *text, friendinfo *info, char *nicheng_times)
{
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(info->show_buffer, &start, &end);
    gtk_text_buffer_insert_with_tags_by_name(info->show_buffer, &end,
            nicheng_times, -1, "red_foreground", NULL);
    gtk_text_buffer_insert_with_tags_by_name(info->show_buffer, &end,
            text, -1, "gray_foreground", NULL);

    gtk_text_buffer_insert_with_tags_by_name(info->show_buffer, &end,
            "\n", -1, "gray_foreground", NULL);

}

//将服务器发过来的的消息显示在文本框上
void Show_remote_text(const gchar *rcvd_text, friendinfo *info)
{
    GtkTextIter start, end;
    char nicheng_times[40] = {0};
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    sprintf(nicheng_times, " %s  %d: %d: %d \n", info->user.nickName, p->tm_hour, p->tm_min, p->tm_sec);
    GtkTextBuffer *show_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (info->show_text));
    gtk_text_buffer_get_bounds(show_buffer, &start, &end);

    // gtk_text_buffer_apply_tag (info->show_buffer, tag, &start, &end);
    gtk_text_buffer_insert_with_tags_by_name(show_buffer, &end,
            nicheng_times, -1, "blue_foreground", NULL);
    gtk_text_buffer_insert_with_tags_by_name(show_buffer, &end,
            rcvd_text, -1, "gray_foreground", NULL);

    gtk_text_buffer_insert_with_tags_by_name(show_buffer, &end,
            "\n", -1, "gray_foreground", NULL);


}


//将输入的内容添加到输入文本框的缓冲区去并取出内容传给显示文本框
void send_text(friendinfo *info)
{
    GtkTextIter start, end;
    GdkPixbuf *pixbuf;
    gchar *char_text;
    char_text = (gchar *) malloc(1024);
    if (char_text == NULL)
    {
        printf("Malloc error!\n");
        exit(1);
    }
//
//    pixbuf = gdk_pixbuf_new_from_file("ss.png", NULL);
//    gtk_text_buffer_insert_pixbuf(info->input_buffer, &start, pixbuf);
    gtk_text_buffer_get_bounds(info->input_buffer, &start, &end);
    char_text = gtk_text_buffer_get_text(info->input_buffer, &start, &end, FALSE);
    g_print("%s\n", char_text);
    char nicheng_times[40] = {0};
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    sprintf(nicheng_times, " %s  %d : %d: %d \n", info->user.nickName, p->tm_hour, p->tm_min, p->tm_sec);
    gtk_text_buffer_set_text(info->input_buffer, "", 0);//发送消息后本地的文本框清0
    CRPMessageNormalSend(sockfd, info->user.uid, UMT_TEXT, info->user.uid, strlen(char_text), char_text);
    show_local_text(char_text, info, nicheng_times);
    free(char_text);


}

//背景的eventbox
static gint chartbackground_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    //设置在非按钮区域内移动窗口
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                event->x_root, event->y_root, event->time);
    }
    return 0;

}

//发送
//鼠标点击事件
static gint send_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    friendinfo *info = (friendinfo *) data;
//    X = event->x;  // 取得鼠标相对于窗口的位置
//    Y = event->y;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagesend, surfacesend2); //置换图标
    }

    return 0;
}

//发送
//鼠标抬起事件
static gint send_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {

        gtk_image_set_from_surface((GtkImage *) info->imagesend, surfacesend1);
        send_text(info);

    }
    return 0;

}

//发送
//鼠标移动事件
static gint send_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    //设置发送按钮
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagesend, surfacesend2); //置换图标

    return 0;
}

//设置离开组件事件
static gint send_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagesend, surfacesend1);
}

//声音
//鼠标点击事件
static gint voice_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    if (event->button == 1)
    {   //设置语音按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice2); //置换图标
    }

    return 0;
}

//声音
//鼠标抬起事件
static gint voice_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice1);
    }
    return 0;

}

//声音
//鼠标移动事件
static gint voice_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    //设置语音按钮
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice2); //置换图标

    return 0;
}

//声音
//设置离开组件事件
static gint voice_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    //设置语音按钮
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice1);

    return 0;
}

//视频
//鼠标点击事件
static gint video_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1)
    {   //设置视频按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagevideo, surfacevideo2); //置换图标
    }

    return 0;
}

//视频
//鼠标抬起事件
static gint video_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imagevideo, surfacevideo1);
    }
    return 0;

}

//视频
//鼠标移动事件
static gint video_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagevideo, surfacevideo2); //置换图标
    return 0;
}

//视频
//鼠标likai事件
static gint video_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagevideo, surfacevideo1);
    return 0;
}

//下方关闭
//鼠标点击事件
static gint close_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1)
    {          //设置右下关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imageclose, surfaceclose2); //置换图标
    }
    return 0;
}

//下方关闭
//鼠标抬起事件
static gint close_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imageclose, surfaceclose1);//设置右下关闭
        gtk_widget_destroy(info->chartwindow);
        info->chartwindow = NULL;
    }
    return 0;
}

//下方关闭
//鼠标移动事件
static gint close_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imageclose, surfaceclose2); //置换图标 //置换图标
    return 0;
}

//下方关闭
//鼠标likai事件
static gint close_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imageclose, surfaceclose1);//设置右下关闭

    return 0;
}

//右上关闭
//鼠标点击事件
static gint close_but_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    if (event->button == 1)
    {         //设置右上关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imageclosebut, surfaceclosebut2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint close_but_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imageclosebut, surfaceclosebut1);  //设置右上关闭按钮
        gtk_widget_destroy(info->chartwindow);
        info->chartwindow = NULL;
    }
    return 0;

}

//鼠标移动事件
static gint close_but_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imageclosebut, surfaceclosebut3); //置换图标
    return 0;
}

//鼠标likai事件
static gint close_but_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    gtk_image_set_from_surface((GtkImage *) info->imageclosebut, surfaceclosebut1);  //设置右上关闭按钮
    return 0;
}

//表情
//鼠标点击事件
static gint look_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagelook, surfacelook2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint look_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imagelook, surfacelook1);
    }
    return 0;

}

//鼠标移动事件
static gint look_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagelook, surfacelook2); //置换图标
    return 0;
}

//鼠标likai事件
static gint look_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagelook, surfacelook1);
    return 0;
}

//jietu
//鼠标点击事件
static gint jietu_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1 && (X > 391 && X < 473) && (Y > 513 && Y < 540))
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagejietu, surfacejietu2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint jietu_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imagejietu, surfacejietu1);
    }
    return 0;

}

//鼠标移动事件
static gint jietu_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagejietu, surfacejietu2); //置换图标

    return 0;
}

//鼠标likai事件
static gint jietu_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagejietu, surfacejietu1);

    return 0;
}

//wenjian
//鼠标点击事件
static gint file_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagefile, surfacefile2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint file_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imagefile, surfacefile1);
    }
    return 0;

}

//鼠标移动事件
static gint file_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagefile, surfacefile2); //置换图标
    return 0;
}

//鼠标likai事件
static gint file_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagefile, surfacefile1);

    return 0;
}

//tupian
//鼠标点击事件
static gint photo_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagephoto, surfaceimage2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint photo_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imagephoto, surfaceimage1);
    }
    return 0;

}

//鼠标移动事件
static gint photo_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagephoto, surfaceimage2); //置换图标

    return 0;
}

//鼠标likai事件
static gint photo_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagephoto, surfaceimage1);
    return 0;
}

//ziti
//鼠标点击事件
static gint wordart_button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagewordart, surfacewordart2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint wordart_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imagewordart, surfacewordart1);
    }
    return 0;

}

//鼠标移动事件
static gint wordart_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagewordart, surfacewordart2); //置换图标
    return 0;
}


//鼠标likai事件
static gint wordart_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{
    friendinfo *info = (friendinfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagewordart, surfacewordart1);
    return 0;
}


int mainchart(friendinfo *friendinfonode)
{

    GtkEventBox *chartbackground_event_box, *send_event_box, *voice_event_box, *video_event_box;
    GtkEventBox *close_event_box, *close_but_event_box, *look_event_box, *jietu_event_box, *file_event_box;
    GtkEventBox *photo_event_box, *wordart_event_box;

    //创建窗口，并为窗口的关闭信号加回调函数以便退出
    friendinfonode->chartwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    friendinfonode->chartlayout = gtk_fixed_new();
    friendinfonode->chartlayout2 = gtk_layout_new(NULL, NULL);

    gtk_container_add(GTK_CONTAINER (friendinfonode->chartwindow), friendinfonode->chartlayout2);//chartlayout2 加入到window
    gtk_container_add(GTK_CONTAINER (friendinfonode->chartlayout2), friendinfonode->chartlayout);

    g_signal_connect(G_OBJECT(friendinfonode->chartwindow), "delete_event",
            G_CALLBACK(gtk_main_quit), NULL);

    gtk_window_set_default_size(GTK_WINDOW(friendinfonode->chartwindow), 500, 550);
    gtk_window_set_position(GTK_WINDOW(friendinfonode->chartwindow), GTK_WIN_POS_CENTER);//窗口出现位置
    //gtk_window_set_resizable (GTK_WINDOW (window), FALSE);//窗口不可改变

    gtk_window_set_decorated(GTK_WINDOW(friendinfonode->chartwindow), FALSE);   // 去掉边框
    create_surfaces(friendinfonode);

    friendinfonode->imageflowerbackgroud = gtk_image_new_from_surface(schartbackgroud);
    friendinfonode->imagesend = gtk_image_new_from_surface(surfacesend1);
    friendinfonode->imagevoice = gtk_image_new_from_surface(surfacevoice1);
    friendinfonode->imagevideo = gtk_image_new_from_surface(surfacevideo1);
    friendinfonode->imageclose = gtk_image_new_from_surface(surfaceclose1);
    friendinfonode->imageclosebut = gtk_image_new_from_surface(surfaceclosebut1);
    friendinfonode->imagelook = gtk_image_new_from_surface(surfacelook1);
    friendinfonode->imagejietu = gtk_image_new_from_surface(surfacejietu1);
    friendinfonode->imagefile = gtk_image_new_from_surface(surfacefile1);
    friendinfonode->imagephoto = gtk_image_new_from_surface(surfaceimage1);
    friendinfonode->imagewordart = gtk_image_new_from_surface(surfacewordart1);


// 设置窗体获取鼠标事件 背景

    chartbackground_event_box = BuildEventBox(
            friendinfonode->imageflowerbackgroud,
            G_CALLBACK(chartbackground_button_press_event),
            NULL, NULL, NULL, friendinfonode);

    //发送

    send_event_box = BuildEventBox(
            friendinfonode->imagesend,
            G_CALLBACK(send_button_press_event),
            G_CALLBACK(send_enter_notify_event),
            G_CALLBACK(send_leave_notify_event),
            G_CALLBACK(send_button_release_event),
            friendinfonode);
    //语音


    voice_event_box = BuildEventBox(
            friendinfonode->imagevoice,
            G_CALLBACK(voice_button_press_event),
            G_CALLBACK(voice_enter_notify_event),
            G_CALLBACK(voice_leave_notify_event),
            G_CALLBACK(voice_button_release_event),
            friendinfonode);
    //视频

    video_event_box = BuildEventBox(
            friendinfonode->imagevideo,
            G_CALLBACK(video_button_press_event),
            G_CALLBACK(video_enter_notify_event),
            G_CALLBACK(video_leave_notify_event),
            G_CALLBACK(video_button_release_event),
            friendinfonode);
//下方关闭按钮

    close_event_box = BuildEventBox(
            friendinfonode->imageclose,
            G_CALLBACK(close_button_press_event),
            G_CALLBACK(close_enter_notify_event),
            G_CALLBACK(close_leave_notify_event),
            G_CALLBACK(close_button_release_event),
            friendinfonode);

//右上方关闭按钮
    close_but_event_box = BuildEventBox(
            friendinfonode->imageclosebut,
            G_CALLBACK(close_but_button_press_event),
            G_CALLBACK(close_but_enter_notify_event),
            G_CALLBACK(close_but_leave_notify_event),
            G_CALLBACK(close_but_button_release_event),
            friendinfonode);
//表情

    look_event_box = BuildEventBox(
            friendinfonode->imagelook,
            G_CALLBACK(look_button_press_event),
            G_CALLBACK(look_enter_notify_event),
            G_CALLBACK(look_leave_notify_event),
            G_CALLBACK(look_button_release_event),
            friendinfonode);

    //截图

    jietu_event_box = BuildEventBox(
            friendinfonode->imagejietu,
            G_CALLBACK(jietu_button_press_event),
            G_CALLBACK(jietu_enter_notify_event),
            G_CALLBACK(jietu_leave_notify_event),
            G_CALLBACK(jietu_button_release_event),
            friendinfonode);
//文件

    file_event_box = BuildEventBox(
            friendinfonode->imagefile,
            G_CALLBACK(file_button_press_event),
            G_CALLBACK(file_enter_notify_event),
            G_CALLBACK(file_leave_notify_event),
            G_CALLBACK(file_button_release_event),
            friendinfonode);

    //图片

    photo_event_box = BuildEventBox(
            friendinfonode->imagephoto,
            G_CALLBACK(photo_button_press_event),
            G_CALLBACK(photo_enter_notify_event),
            G_CALLBACK(photo_leave_notify_event),
            G_CALLBACK(photo_button_release_event),
            friendinfonode);
    //字体

    wordart_event_box = BuildEventBox(
            friendinfonode->imagewordart,
            G_CALLBACK(wordart_button_press_event),
            G_CALLBACK(wordart_enter_notify_event),
            G_CALLBACK(wordart_leave_notify_event),
            G_CALLBACK(wordart_button_release_event),
            friendinfonode);

//背景
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), chartbackground_event_box, 0, 0);
    //逆臣
    GtkWidget *nicheng;
    nicheng = gtk_label_new(friendinfonode->user.nickName);
    //daxiao
    PangoFontDescription *font;
    font = pango_font_description_from_string("Sans");//"Sans"字体名
    pango_font_description_set_size(font, 20 * PANGO_SCALE);//设置字体大小
    gtk_widget_override_font(nicheng, font);
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), nicheng, 100, 20);

//发送
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), send_event_box, 390, 510);
//语音
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), voice_event_box, 80, 50);
//视频按钮
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), video_event_box, 120, 50);
//下方关闭按钮
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), close_event_box, 300, 512);
//右上角关闭按钮
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), close_but_event_box, 470, 0);
//表情
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), look_event_box, 40, 405);
//截图
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), jietu_event_box, 165, 405);
//文件
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), file_event_box, 125, 405);
//图片
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), photo_event_box, 80, 405);
//字体
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), wordart_event_box, 5, 405);
//头像
    friendinfonode->imagehead3 = gtk_image_new_from_surface(surfacehead3);
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), friendinfonode->imagehead3, 15, 8);

    //创建发送文本框，和接受文本框
    friendinfonode->input_text = gtk_text_view_new();
    friendinfonode->show_text = gtk_text_view_new();

    friendinfonode->input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (friendinfonode->input_text));
    friendinfonode->show_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (friendinfonode->show_text));

    //创建文字标记
    gtk_text_buffer_create_tag(friendinfonode->show_buffer, "red_foreground", "foreground", "red", NULL);
    gtk_text_buffer_create_tag(friendinfonode->show_buffer, "gray_foreground", "foreground", "gray", NULL);
    gtk_text_buffer_create_tag(friendinfonode->show_buffer, "blue_foreground", "foreground", "blue", NULL);

    //创建滚动窗口
    friendinfonode->sw1 = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    friendinfonode->sw2 = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));

    gtk_container_add(GTK_CONTAINER(friendinfonode->sw1), friendinfonode->input_text);
    gtk_container_add(GTK_CONTAINER(friendinfonode->sw2), friendinfonode->show_text);

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(friendinfonode->input_text), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(friendinfonode->show_text), GTK_WRAP_WORD_CHAR);//自动换行

    gtk_text_view_set_editable(GTK_TEXT_VIEW(friendinfonode->show_text),
            0);//不可编辑
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(friendinfonode->show_text), FALSE);

    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(friendinfonode->sw1), 0, 440);//文本框位置
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(friendinfonode->sw2), 0, 90);

    gtk_widget_set_size_request(GTK_WIDGET(friendinfonode->sw1), 500, 65);
    gtk_widget_set_size_request(GTK_WIDGET(friendinfonode->sw2), 500, 317);//大小

    GdkRGBA rgba = {1, 1, 1, 0.2};
    gtk_widget_override_background_color(friendinfonode->input_text, GTK_STATE_NORMAL, &rgba);//设置透明
    gtk_widget_override_background_color(friendinfonode->show_text, GTK_STATE_NORMAL, &rgba);//设置透明


    gtk_widget_show_all(friendinfonode->chartwindow);

    // gtk_main();

    return 0;
}