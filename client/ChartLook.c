#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <stdlib.h>
#include "chart.h"
#include "common.h"
#include <pwd.h>
#include <math.h>
#include <string.h>
#include <cairo-script-interpreter.h>
#include "chartmessage.h"
#include "ChartRecord.h"
#include "ScreenShot.h"

static gint face_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    const char *path = g_object_get_data(G_OBJECT(widget), "ImageSrc");
    char *filename = (gchar *) malloc(256);
    size_t filenamelen;
    sprintf(filename, "/opt/momo/face/%s", path);
    GtkWidget *image;
    image = gtk_image_new_from_file(filename);
    GtkTextMark *mark;
    GtkTextIter iter, end;
    GtkTextChildAnchor *anchor;
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
    gtk_text_buffer_get_end_iter(info->input_buffer, &end);
    GtkTextMark *text_mark_log = gtk_text_buffer_create_mark(info->input_buffer, NULL, &iter, 1);
    gtk_text_buffer_move_mark(info->input_buffer, text_mark_log, &end);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(info->input_text), text_mark_log, 0, 1, 1, 1);
    if (info->look_window != NULL)
    {
        gtk_widget_destroy(info->look_window);
        info->look_window = NULL;
    }
    return 0;
}

void ChartLook(FriendInfo *info, gdouble event_x, gdouble event_y)
{
    GtkWidget *smile, *goodbye, *lovely, *angry, *cry, *poor;
    GtkWidget *shy, *nose, *laughter;
    GtkEventBox *smile_event_box, *goodbye_event_box, *lovely_event_box, *angry_event_box, *cry_event_box;
    GtkEventBox *shy_event_box, *poor_event_box, *nose_event_box, *laughter_event_box;
    info->look_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_move(GTK_WINDOW(info->look_window), event_x, event_y);
    gtk_window_set_decorated(GTK_WINDOW(info->look_window), FALSE);   // 去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(info->look_window), 100, 100);

    info->look_layout = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(info->look_window), info->look_layout);

    GdkRGBA rgbacolor;
    rgbacolor.alpha = 1;
    rgbacolor.red = 1;
    rgbacolor.green = 1;
    rgbacolor.blue = 1;
    gtk_widget_override_background_color(info->look_layout, GTK_STATE_FLAG_NORMAL, &rgbacolor); //设置背景
    gtk_widget_set_opacity(info->look_layout, 0.8); //设置窗口全透明

    smile = ChangeFace_file("1.png");
    goodbye = ChangeFace_file("2.png");
    lovely = ChangeFace_file("3.png");
    angry = ChangeFace_file("4.png");
    cry = ChangeFace_file("5.png");
    poor = ChangeFace_file("6.png");
    shy = ChangeFace_file("7.png");
    nose = ChangeFace_file("8.png");
    laughter = ChangeFace_file("9.png");

    smile_event_box = BuildEventBox(
            smile,
            G_CALLBACK(face_button_press_event),
            NULL, NULL, NULL, NULL, info);
    g_object_set_data(G_OBJECT(smile_event_box), "ImageSrc", "1.png");
    goodbye_event_box = BuildEventBox(
            goodbye,
            G_CALLBACK(face_button_press_event),
            NULL, NULL, NULL, NULL, info);
    g_object_set_data(G_OBJECT(goodbye_event_box), "ImageSrc", "2.png");
    lovely_event_box = BuildEventBox(
            lovely,
            G_CALLBACK(face_button_press_event),
            NULL, NULL, NULL, NULL, info);
    g_object_set_data(G_OBJECT(lovely_event_box), "ImageSrc", "3.png");
    angry_event_box = BuildEventBox(
            angry,
            G_CALLBACK(face_button_press_event),
            NULL, NULL, NULL, NULL, info);
    g_object_set_data(G_OBJECT(angry_event_box), "ImageSrc", "4.png");
    cry_event_box = BuildEventBox(
            cry,
            G_CALLBACK(face_button_press_event),
            NULL, NULL, NULL, NULL, info);
    g_object_set_data(G_OBJECT(cry_event_box), "ImageSrc", "5.png");
    poor_event_box = BuildEventBox(
            poor,
            G_CALLBACK(face_button_press_event),
            NULL, NULL, NULL, NULL, info);
    g_object_set_data(G_OBJECT(poor_event_box), "ImageSrc", "6.png");
    shy_event_box = BuildEventBox(
            shy,
            G_CALLBACK(face_button_press_event),
            NULL, NULL, NULL, NULL, info);
    g_object_set_data(G_OBJECT(shy_event_box), "ImageSrc", "7.png");
    nose_event_box = BuildEventBox(
            nose,
            G_CALLBACK(face_button_press_event),
            NULL, NULL, NULL, NULL, info);
    g_object_set_data(G_OBJECT(nose_event_box), "ImageSrc", "8.png");
    laughter_event_box = BuildEventBox(
            laughter,
            G_CALLBACK(face_button_press_event),
            NULL, NULL, NULL, NULL, info);
    g_object_set_data(G_OBJECT(laughter_event_box), "ImageSrc", "9.png");

    gtk_fixed_put(GTK_FIXED(info->look_layout), GTK_WIDGET(smile_event_box), 0, 0);
    gtk_fixed_put(GTK_FIXED(info->look_layout), GTK_WIDGET(lovely_event_box), 40, 0);
    gtk_fixed_put(GTK_FIXED(info->look_layout), GTK_WIDGET(angry_event_box), 80, 0);
    gtk_fixed_put(GTK_FIXED(info->look_layout), GTK_WIDGET(goodbye_event_box), 0, 40);
    gtk_fixed_put(GTK_FIXED(info->look_layout), GTK_WIDGET(laughter_event_box), 40, 40);
    gtk_fixed_put(GTK_FIXED(info->look_layout), GTK_WIDGET(shy_event_box), 80, 40);
    gtk_fixed_put(GTK_FIXED(info->look_layout), GTK_WIDGET(poor_event_box), 0, 80);
    gtk_fixed_put(GTK_FIXED(info->look_layout), GTK_WIDGET(cry_event_box), 40, 80);
    gtk_fixed_put(GTK_FIXED(info->look_layout), GTK_WIDGET(nose_event_box), 80, 80);
    gtk_widget_show_all(info->look_window);
}