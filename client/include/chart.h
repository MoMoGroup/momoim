#pragma once
#include "ClientSockfd.h"
#include <gtk/gtk.h>

struct UserTextInformation
{
    gchar *font;
    PangoStyle style;
    PangoWeight weight;
    gint size;
    gchar *coding_font_color;
    PangoFontDescription *description;
    guint16 color_red;
    guint16 color_green;
    guint16 color_blue;
    int codinglen;
};  //存放用户字体的结构体
extern struct UserTextInformation UserWordInfo;

extern void LoadingIcon(FriendInfo *info);

int OnAudioCloseMsg(gpointer p);
int OnAudioRefuseMsg(gpointer p);
gboolean ProcessAudioRequest(gpointer user_data);