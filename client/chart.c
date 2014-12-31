#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include "ClientSockfd.h"
#include <cairo.h>
#include <logger.h>
#include <imcommon/friends.h>
#include <stdlib.h>
#include <ftadvanc.h>
#include <gmodule.h>
#include <time.h>
#include "chart.h"
int X = 0;
int Y = 0;


static cairo_surface_t *sflowerbackgroud, *surfacesend1, *surfacesend2, *surfacehead3, *surfacevoice1, *surfacevoice2, *surfacevideo1, *surfacevideo2;
static cairo_surface_t *surfaceclose1, *surfaceclose2, *surfaceclosebut1, *surfaceclosebut2, *surfaceclosebut3;


static void create_surfaces(friendinfo *information) {
    if (sflowerbackgroud == NULL) {
        sflowerbackgroud = cairo_image_surface_create_from_png("花背景.png");
        surfacesend1 = cairo_image_surface_create_from_png("发送1.png");
        surfacesend2 = cairo_image_surface_create_from_png("发送2.png");
        surfacehead3 = cairo_image_surface_create_from_png("头像3.png");
        surfacevoice1 = cairo_image_surface_create_from_png("语音1.png");
        surfacevoice2 = cairo_image_surface_create_from_png("语音2.png");
        surfacevideo2 = cairo_image_surface_create_from_png("视频1.png");
        surfacevideo1 = cairo_image_surface_create_from_png("视频2.png");
        surfaceclose1 = cairo_image_surface_create_from_png("关闭1.png");
        surfaceclose2 = cairo_image_surface_create_from_png("关闭2.png");
        surfaceclosebut1 = cairo_image_surface_create_from_png("关闭按钮1.png");
        surfaceclosebut2 = cairo_image_surface_create_from_png("关闭按钮2.png");
        surfaceclosebut3 = cairo_image_surface_create_from_png("关闭按钮3.png");
    }

    information->imageflowerbackgroud = gtk_image_new_from_surface(sflowerbackgroud);
    gtk_fixed_put(GTK_FIXED(information->chartlayout), information->imageflowerbackgroud, 0, 0);
//发送
    information->imagesend = gtk_image_new_from_surface(surfacesend1);
    gtk_fixed_put(GTK_FIXED(information->chartlayout), information->imagesend, 390, 510);

//头像
    information->imagehead3 = gtk_image_new_from_surface(surfacehead3);
    gtk_fixed_put(GTK_FIXED(information->chartlayout), information->imagehead3, 0, 0);

//语音
    information->imagevoice = gtk_image_new_from_surface(surfacevoice1);
    gtk_fixed_put(GTK_FIXED(information->chartlayout), information->imagevoice, 15, 70);


//视频按钮
    information->imagevideo = gtk_image_new_from_surface(surfacevideo2);
    gtk_fixed_put(GTK_FIXED(information->chartlayout), information->imagevideo, 60, 70);


//下方关闭按钮
    information->imageclose = gtk_image_new_from_surface(surfaceclose1);
    gtk_fixed_put(GTK_FIXED(information->chartlayout), information->imageclose, 300, 512);


//右上角关闭按钮
    information->imageclosebut = gtk_image_new_from_surface(surfaceclosebut1);
    gtk_fixed_put(GTK_FIXED(information->chartlayout), information->imageclosebut, 470, 0);

    //xianshinicheng GtkWidget *charttowho;

    GtkWidget *nicheng;
    nicheng = gtk_label_new(information->user.nickName);
    //daxiao
    PangoFontDescription *font;
    font = pango_font_description_from_string("Sans");//"Sans"字体名
    pango_font_description_set_size(font, 20 * PANGO_SCALE);//设置字体大小
    gtk_widget_override_font(nicheng, font);

    gtk_fixed_put(GTK_FIXED(information->chartlayout), nicheng, 75, 20);

}
//
//static void
//destroy_surfaces3() {
//    g_print("destroying surfaces3");
//
//    cairo_surface_destroy(sflowerbackgroud);
//    cairo_surface_destroy(surfacesend1);
//    cairo_surface_destroy(surfacesend2);
//    cairo_surface_destroy(surfacehead3);
//    cairo_surface_destroy(surfacevoice1);
//    cairo_surface_destroy(surfacevoice2);
//    cairo_surface_destroy(surfacevideo2);
//    cairo_surface_destroy(surfacevideo1);
//    cairo_surface_destroy(surfaceclose1);
//    cairo_surface_destroy(surfaceclose2);
//    cairo_surface_destroy(surfaceclosebut1);
//    cairo_surface_destroy(surfaceclosebut2);
//    cairo_surface_destroy(surfaceclosebut3);
//}

//将输入的文本框输出在显示的文本框中
void show_local_text(const gchar *text, friendinfo *info, char *nicheng_times) {
    GtkTextIter start, end;


    gtk_text_buffer_get_bounds(info->show_buffer, &start, &end);
    gtk_text_buffer_create_tag(info->show_buffer, "red_foreground", "foreground", "red", NULL);
    gtk_text_buffer_create_tag(info->show_buffer, "gray_foreground", "foreground", "gray", NULL);
    // gtk_text_buffer_apply_tag (info->show_buffer, tag, &start, &end);
    gtk_text_buffer_insert_with_tags_by_name(info->show_buffer, &start,
            nicheng_times, -1, "red_foreground", NULL);
    gtk_text_buffer_insert_with_tags_by_name(info->show_buffer, &start,
            text, -1, "gray_foreground", NULL);

    gtk_text_buffer_insert_with_tags_by_name(info->show_buffer, &start,
            "\n", -1, "gray_foreground", NULL);


}

//将服务器发过来的的消息显示在文本框上
void show_remote_text(const gchar *rcvd_text,friendinfo *info){
    GtkTextIter start,end;
    char nicheng_times[40] = {0};
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    sprintf(nicheng_times, " %s  %d: %d: %d \n", info->user.nickName, p->tm_hour, p->tm_min, p->tm_sec);
    gtk_text_buffer_get_bounds(info->show_buffer, &start, &end);
    gtk_text_buffer_create_tag(info->show_buffer, "blue_foreground", "foreground", "blue", NULL);
    gtk_text_buffer_create_tag(info->show_buffer, "gray_foreground", "foreground", "gray", NULL);
    // gtk_text_buffer_apply_tag (info->show_buffer, tag, &start, &end);
    gtk_text_buffer_insert_with_tags_by_name(info->show_buffer, &start,
            nicheng_times, -1, "blue_foreground", NULL);
    gtk_text_buffer_insert_with_tags_by_name(info->show_buffer, &start,
            rcvd_text, -1, "gray_foreground", NULL);

    gtk_text_buffer_insert_with_tags_by_name(info->show_buffer, &start,
            "\n", -1, "gray_foreground", NULL);
}


//将输入的内容添加到输入文本框的缓冲区去并取出内容传给显示文本框
void send_text(friendinfo *info) {
    GtkTextIter start, end;
    gchar *char_text;
    char_text = (gchar *) malloc(1024);
    if (char_text == NULL) {
        printf("Malloc error!\n");
        exit(1);
    }
    gtk_text_buffer_get_bounds(info->input_buffer, &start, &end);
    char_text = gtk_text_buffer_get_text(info->input_buffer, &start, &end, FALSE);
    g_print("%s\n", char_text);
    char nicheng_times[40] = {0};
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    sprintf(nicheng_times, " %s  %d : %d: %d \n", info->user.nickName, p->tm_hour, p->tm_min, p->tm_sec);
    CRPMessageNormalSend(sockfd, info->user.uid, UMT_TEXT, info->user.uid, strlen(char_text), char_text);
    show_local_text(char_text, info, nicheng_times);
    free(char_text);
}

//鼠标点击事件
static gint button_press_event(GtkWidget *widget,

        GdkEventButton *event, gpointer data) {
    friendinfo *info = (friendinfo *) data;
    X = event->x;  // 取得鼠标相对于窗口的位置
    Y = event->y;

    if (event->button == 1 && (X > 391 && X < 473) && (Y > 513 && Y < 540)) {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagesend, surfacesend2); //置换图标
    }
    else if (event->button == 1 && (X > 18 && X < 43) && (Y > 70 && Y < 100)) {   //设置语音按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice2); //置换图标
    }
    else if (event->button == 1 && (X > 60 && X < 93) && (Y > 74 && Y < 103)) {   //设置视频按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagevideo, surfacevideo1); //置换图标
    }
    else if (event->button == 1 && (X > 301 && X < 382) && (Y > 513 && Y < 540)) {          //设置右下关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imageclose, surfaceclose2); //置换图标
    }
    else if (event->button == 1 && (X > 470 && X < 500) && (Y > 2 && Y < 25)) {         //设置右上关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imageclosebut, surfaceclosebut2); //置换图标
    }
    else {
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
        if (event->button == 1) { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
            gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                    event->x_root, event->y_root, event->time);
        }
    }


    return 0;
}

//鼠标抬起事件
static gint button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data) {
    friendinfo *info = (friendinfo *) data;
    X = event->x;  // 取得鼠标相对于窗口的位置
    Y = event->y;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice1);
        gtk_image_set_from_surface((GtkImage *) info->imagevideo, surfacevideo2);
        gtk_image_set_from_surface((GtkImage *) info->imageclose, surfaceclose1);//设置右下关闭
        gtk_image_set_from_surface((GtkImage *) info->imageclosebut, surfaceclosebut1);  //设置右上关闭按钮

        if ((X > 391 && X < 473) && (Y > 513 && Y < 540)) {
            gtk_image_set_from_surface((GtkImage *) info->imagesend, surfacesend1);
            GtkWidget *dialog;
            //创建带确认按钮的对话框，父控件为空
            dialog = gtk_message_dialog_new(NULL,
                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "%s",info->user.nickName);//到时候可以显示出昵称

            gtk_dialog_run(GTK_DIALOG(dialog));//显示并运行对话框

            send_text(info);
            gtk_widget_destroy(dialog);//销毁对话框
        }
        if (((X > 470 && X < 500) && (Y > 2 && Y < 25)) || ((X > 301 && X < 382) && (Y > 513 && Y < 540))) {

            //DeleteEvent();
            gtk_widget_destroy(info->chartwindow);
            info->chartwindow = NULL;
        }

    }
    return 0;

}

//鼠标移动事件
static gint motion_notify_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data) {
    friendinfo *info = (friendinfo *) data;

    X = event->x;  // 取得鼠标相对于窗口的位置
    Y = event->y;
    if ((X > 391 && X < 473) && (Y > 513 && Y < 540)) {     //设置发送按钮

        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagesend, surfacesend2); //置换图标
    }
    else if ((X > 18 && X < 43) && (Y > 70 && Y < 100)) {   //设置语音按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice2); //置换图标
    }
    else if ((X > 60 && X < 93) && (Y > 74 && Y < 103)) {   //设置视频按钮

        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagevideo, surfacevideo1); //置换图标
    }
    else if ((X > 301 && X < 382) && (Y > 513 && Y < 540)) {          //设置右下关闭按钮

        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imageclose, surfaceclose2); //置换图标 //置换图标
    }
    else if ((X > 470 && X < 500) && (Y > 2 && Y < 25)) {         //设置右上关闭按钮

        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imageclosebut, surfaceclosebut3); //置换图标
    }
    else {
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
        gtk_image_set_from_surface((GtkImage *) info->imagesend, surfacesend1);
        gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice1);
        gtk_image_set_from_surface((GtkImage *) info->imagevideo, surfacevideo2);
        gtk_image_set_from_surface((GtkImage *) info->imageclose, surfaceclose1);//设置右下关闭
        gtk_image_set_from_surface((GtkImage *) info->imageclosebut, surfaceclosebut1);  //设置右上关闭按钮
    }

    return 0;
}

int mainchart(friendinfo *friendinfonode) {


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
// 设置窗体获取鼠标事件
    gtk_widget_set_events(friendinfonode->chartwindow,

            GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK

                    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK

                    | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

    g_signal_connect(G_OBJECT(friendinfonode->chartwindow), "button_press_event",
            G_CALLBACK(button_press_event), friendinfonode);       // 加入事件回调
    g_signal_connect(G_OBJECT(friendinfonode->chartwindow), "motion_notify_event",
            G_CALLBACK(motion_notify_event), friendinfonode);

    g_signal_connect(G_OBJECT(friendinfonode->chartwindow), "button_release_event",
            G_CALLBACK(button_release_event), friendinfonode);

    create_surfaces(friendinfonode);

    //创建发送文本框，和接受文本框
    friendinfonode->input_text = gtk_text_view_new();
    friendinfonode->show_text = gtk_text_view_new();

    friendinfonode->input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (friendinfonode->input_text));
    friendinfonode->show_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (friendinfonode->show_text));

    friendinfonode->sw1 = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    friendinfonode->sw2 = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));

    gtk_container_add(GTK_CONTAINER(friendinfonode->sw1), friendinfonode->input_text);
    gtk_container_add(GTK_CONTAINER(friendinfonode->sw2), friendinfonode->show_text);

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(friendinfonode->input_text), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(friendinfonode->show_text), GTK_WRAP_WORD_CHAR);//自动换行

    gtk_text_view_set_editable(GTK_TEXT_VIEW(friendinfonode->show_text),
            0);//不可编辑
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(friendinfonode->show_text), FALSE);

    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(friendinfonode->sw1), 0, 425);//文本框位置
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(friendinfonode->sw2), 0, 115);

    gtk_widget_set_size_request(GTK_WIDGET(friendinfonode->sw1), 500, 80);
    gtk_widget_set_size_request(GTK_WIDGET(friendinfonode->sw2), 500, 300);//大小

    GdkRGBA rgba = {1, 1, 1, 0};
    gtk_widget_override_background_color(friendinfonode->input_text, GTK_STATE_NORMAL, &rgba);//设置透明
    gtk_widget_override_background_color(friendinfonode->show_text, GTK_STATE_NORMAL, &rgba);//设置透明


    gtk_widget_show_all(friendinfonode->chartwindow);

    // gtk_main();

    return 0;
}