#pragma once

#include "ClientSockfd.h"

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


extern int flag_audio_close;

extern void LoadingIcon(FriendInfo *info);
