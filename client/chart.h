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
};
extern struct UserTextInformation UserWordInfo;

typedef struct
{
    gdouble x;
    gdouble y;
    gdouble width;
    gdouble height;
    gboolean press;
} JieTuDATA; //保存鼠标坐标位置的数据结构
int OnAudioCloseMsg(gpointer p);
int OnAudioRefuseMsg(gpointer p);