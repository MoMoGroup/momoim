#pragma once

#include <gtk/gtk.h>
#include "ClientSockfd.h"

extern void ScreenShot(FriendInfo *info);

typedef struct
{
    gdouble x;
    gdouble y;
    gdouble width;
    gdouble height;
    gboolean press;
} JieTuDATA; //保存鼠标坐标位置的数据结构
